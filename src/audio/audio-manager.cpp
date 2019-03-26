#include <oni-core/audio/audio-manager.h>

#include <fmod.hpp>

#include <oni-core/component/geometry.h>

#define ERRCHECK(_result) assert((_result) == FMOD_OK)
#define VALID(MAP, ID) assert(MAP.find(id) != MAP.end())

namespace oni {
    namespace audio {
        AudioManager::AudioManager() {
            mMaxAuidbleDistance = 150.f;

            FMOD::System *system{nullptr};

            auto result = FMOD::System_Create(&system);
            ERRCHECK(result);
            mSystem = std::unique_ptr<FMOD::System, FMODDeleter>(system, FMODDeleter());

            common::uint32 version;
            result = system->getVersion(&version);
            ERRCHECK(result);
            assert(version >= FMOD_VERSION);

            result = system->init(32, FMOD_INIT_NORMAL, nullptr);
            ERRCHECK(result);

            result = system->update();
            ERRCHECK(result);

/*            component::SoundID backgroundSoundID = "resources/audio/beat.wav";
            loadSound(backgroundSoundID);
            attachControls(backgroundSoundID);
            setLoop(backgroundSoundID, true);
            setVolume(backgroundSoundID, 0.2f);
            //playSound(backgroundSoundID);*/

            ENGINE_IDLE = "resources/audio/car/car-2/idle-low-slow.wav";
            loadSound(ENGINE_IDLE);

/*            component::SoundID rocketFastSoundID = "resources/audio/rocket/1-fast.wav";
            loadSound(rocketFastSoundID);

            component::SoundID rocketSlowSoundID = "resources/audio/rocket/1-slow.wav";
            loadSound(rocketSlowSoundID);*/

            ROCKET = "resources/audio/rocket/1-idle.wav";
            loadSound(ROCKET);
        }

        void AudioManager::tick(entities::EntityFactory &entityFactory, const math::vec3 &playerPos) {
            auto result = mSystem->update();
            ERRCHECK(result);

            // Engine
            {
                auto view = entityFactory.getEntityManager().createViewWithLock<component::Tag_Audible,
                        component::Placement, component::Car, component::SoundTag>();
                for (auto &&entity : view) {
                    auto &car = view.get<component::Car>(entity);
                    auto &placement = view.get<component::Placement>(entity);
                    auto &soundTag = view.get<component::SoundTag>(entity);
                    math::vec3 velocity{car.velocity.x, car.velocity.y, 0.f};

                    // TODO: Using soundTag find the correct sound to play.
                    component::SoundID soundID = ENGINE_IDLE;

                    auto &entityPos = placement.position;
                    auto distance = (entityPos - playerPos).len();
                    if (distance < mMaxAuidbleDistance) {
                        auto pitch = static_cast< common::real32>(car.rpm) / 2000;
                        auto &entityChannel = getOrCreateChannelIfMissing(soundID, entity);
                        setPitch(entityChannel, pitch);
                        set3DPos(entityChannel, entityPos - playerPos, velocity);

                        unPause(entityChannel);
                    } else {
                        auto &entityChannel = getOrCreateChannelIfMissing(soundID, entity);
                        pause(entityChannel);
                    }
                }
            }

            {
                auto view = entityFactory.getEntityManager().createViewWithLock<component::Tag_Audible,
                        component::Placement, component::SoundTag>();
                for (auto &&entity : view) {
                    auto &pos = view.get<component::Placement>(entity).position;
                    auto &soundTag = view.get<component::SoundTag>(entity);

                    // TODO: Using soundTag find the correct sound to play.
                    if (soundTag != component::SoundTag::ROCKET) {
                        continue;
                    }
                    component::SoundID soundID = ROCKET;

                    auto distance = (pos - playerPos).len();
                    if (distance < mMaxAuidbleDistance) {
                        auto &entityChannel = getOrCreateChannelIfMissing(soundID, entity);

                        // TODO: Does it matter if this is accurate?
                        math::vec3 vel{1.f, 0.f, 0.f};
                        set3DPos(entityChannel, pos - playerPos, vel);
                        unPause(entityChannel);
                    } else {
                        auto &entityChannel = getOrCreateChannelIfMissing(soundID, entity);
                        pause(entityChannel);
                    }
                }
            }
        }

        void AudioManager::kill(common::EntityID entityID) {
            for (auto it = mEntityAudioChannel.begin(); it != mEntityAudioChannel.end();) {
                if (it->second.entityID == entityID) {
                    auto result = it->second.channel->stop();
                    ERRCHECK(result);
                    it = mEntityAudioChannel.erase(it);
                } else {
                    ++it;
                }
            }
        }

        void AudioManager::loadSound(const component::SoundID &id) {
            FMOD::Sound *sound{};
            auto result = mSystem->createSound(id.c_str(), FMOD_DEFAULT, nullptr, &sound);
            ERRCHECK(result);
            assert(sound);

            mSounds[id] = std::unique_ptr<FMOD::Sound, FMODDeleter>(sound, FMODDeleter());
        }

        FMOD::Channel *AudioManager::createChannel(const component::SoundID &id) {
            VALID(mSounds, id);

            FMOD::Channel *channel{nullptr};
            auto result = mSystem->playSound(mSounds[id].get(), nullptr, true, &channel);
            ERRCHECK(result);

            return channel;
        }

        void AudioManager::play(const component::SoundID &id, const math::vec2 &distance, common::real32 volume,
                                common::real32 pitch) {
            VALID(mSounds, id);
            auto channel = createChannel(id);
            auto result = channel->setVolume(volume);
            ERRCHECK(result);
            result = channel->setPitch(pitch);
            ERRCHECK(result);

            // TODO: Set position

            result = channel->setPaused(false);
            ERRCHECK(result);

        }

        static FMOD_RESULT
        endOfPlayCallback(FMOD_CHANNELCONTROL *channelControl, FMOD_CHANNELCONTROL_TYPE type,
                          FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void *, void *) {
            if (type == FMOD_CHANNELCONTROL_TYPE::FMOD_CHANNELCONTROL_CHANNELGROUP) {
                return FMOD_OK;
            }

            auto *channel = (FMOD::Channel *) (channelControl);
            if (callbackType == FMOD_CHANNELCONTROL_CALLBACK_TYPE::FMOD_CHANNELCONTROL_CALLBACK_END) {
                void *finished;
                auto result = channel->getUserData(&finished);
                ERRCHECK(result);
                *(bool *) finished = true;
            }

            return FMOD_OK;
        };

        void AudioManager::tryPlay(const component::SoundID &soundID, common::EntityID entityID) {
            auto soundEntityID = getID(soundID, entityID);
            if (mEntityAudioChannel.find(soundEntityID) == mEntityAudioChannel.end()) {
                EntityChannel entityChannel;
                entityChannel.entityID = entityID;
                entityChannel.channel = createChannel(soundID);
                auto result = entityChannel.channel->setVolume(0.5f);
                ERRCHECK(result);

                mEntityAudioChannel[soundEntityID] = std::move(entityChannel);
            }

            auto &entityChannel = mEntityAudioChannel.at(soundEntityID);
            bool paused;
            auto result = entityChannel.channel->getPaused(&paused);
            // NOTE: This will happen if the play-back has ended.
            if (result == FMOD_ERR_INVALID_HANDLE) {
                mEntityAudioChannel.erase(soundEntityID);
                tryPlay(soundID, entityID);
            } else if (paused) {
                result = entityChannel.channel->setPaused(false);
                ERRCHECK(result);
            } else {
                // NOTE: The sound is already playing.
            }
        }

        void AudioManager::kill(const component::SoundID &soundID, common::EntityID entityID) {
            auto soundEntityID = getID(soundID, entityID);
            if (mEntityAudioChannel.find(soundEntityID) == mEntityAudioChannel.end()) {
                assert(false);
                return;
            }

            auto &entityChannel = mEntityAudioChannel.at(soundEntityID);
            auto result = entityChannel.channel->stop();
            ERRCHECK(result);

            mEntityAudioChannel.erase(soundEntityID);
        }

        void AudioManager::fadeOut(const component::SoundID &id) {
            //VALID(mChannels, id);
        }

        AudioManager::SoundEntityID AudioManager::getID(const component::SoundID &soundID,
                                                        common::EntityID entityID) {
            return soundID + std::to_string(entityID);
        }

        void AudioManager::fadeOut(const component::SoundID &soundID, common::EntityID entityID) {
            // TODO: Proper fade-out
            kill(soundID, entityID);
        }

        AudioManager::EntityChannel &
        AudioManager::getOrCreateChannelIfMissing(const component::SoundID &soundID, common::EntityID entityID) {
            auto id = getID(soundID, entityID);
            if (mEntityAudioChannel.find(id) == mEntityAudioChannel.end()) {
                EntityChannel entityChannel;
                entityChannel.entityID = entityID;
                entityChannel.channel = createChannel(soundID);
                // TODO: This should not be happening, assign channel group and set volume on the group
                entityChannel.channel->setVolume(0.1f);
                // TODO: How will you handle non-looping?
                entityChannel.channel->setMode(FMOD_LOOP_NORMAL | FMOD_3D);

                mEntityAudioChannel[id] = entityChannel;
            }
            return mEntityAudioChannel[id];
        }

        void AudioManager::setPitch(AudioManager::EntityChannel &entityChannel, common::real32 pitch) {
            if (pitch > 256) {
                pitch = 256;
            }
            auto result = entityChannel.channel->setPitch(pitch);
            ERRCHECK(result);
        }

        bool AudioManager::isPaused(AudioManager::EntityChannel &entityChannel) {
            bool paused{false};
            auto result = entityChannel.channel->getPaused(&paused);
            ERRCHECK(result);
            return paused;
        }

        void AudioManager::unPause(AudioManager::EntityChannel &entityChannel) {
            auto result = entityChannel.channel->setPaused(false);
            ERRCHECK(result);
        }

        void AudioManager::pause(AudioManager::EntityChannel &entityChannel) {
            auto result = entityChannel.channel->setPaused(true);
            ERRCHECK(result);
        }

        void AudioManager::FMODDeleter::operator()(FMOD::Sound *s) const {
            s->release();
        }

        void AudioManager::FMODDeleter::operator()(FMOD::System *sys) const {
            sys->close();
            sys->release();
        }

        void AudioManager::playCollisionSoundEffect(component::EntityType A, component::EntityType B) {
            auto soundID = createCollisionEffectID(A, B);
            assert(mCollisionEffects.find(soundID) != mCollisionEffects.end());
            //play(mCollisionEffects[soundID]);
        }

        common::UInt16Pack AudioManager::createCollisionEffectID(component::EntityType A, component::EntityType B) {
            static_assert(sizeof(A) == sizeof(common::uint16), "Hashing will fail due to size mismatch");
            auto x = static_cast<common::uint16 >(A);
            auto y = static_cast<common::uint16 >(B);

            if (x > y) {
                std::swap(x, y); // Assuming soundEffect for A->B collision is same as B->A
            }

            auto soundID = math::packUInt16(x, y);
            return soundID;
        }

        void AudioManager::preLoadCollisionSoundEffects() {
            component::SoundID bulletWithUnknown = "resources/audio/collision/bullet-with-unknown.wav";
            loadSound(bulletWithUnknown);
            for (auto i = static_cast<common::uint16 >(component::EntityType::UNKNOWN);
                 i < static_cast<common::uint16>(component::EntityType::LAST);
                 ++i) {
                auto id = createCollisionEffectID(component::EntityType::SIMPLE_ROCKET,
                                                  static_cast<component::EntityType>(i));
                mCollisionEffects[id] = bulletWithUnknown;
            }
        }

        void AudioManager::set3DPos(AudioManager::EntityChannel &entityChannel, const math::vec3 &pos,
                                    const math::vec3 &velocity) {
            FMOD_VECTOR fPos;
            fPos.x = pos.x;
            fPos.y = pos.y;
            fPos.z = pos.z;
            FMOD_VECTOR fVelocity;
            fVelocity.x = velocity.x;
            fVelocity.y = velocity.y;
            fVelocity.z = velocity.z;

            auto result = entityChannel.channel->set3DAttributes(&fPos, &fVelocity);
            ERRCHECK(result);

            result = entityChannel.channel->set3DMinMaxDistance(20.f, mMaxAuidbleDistance); // In meters
            ERRCHECK(result);
        }
    }
}