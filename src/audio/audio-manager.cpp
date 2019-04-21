#include <oni-core/audio/audio-manager.h>

#include <fmod.hpp>

#include <oni-core/component/geometry.h>

#define ERRCHECK(_result) assert((_result) == FMOD_OK)
#define VALID(MAP, ID) assert(MAP.find(id) != MAP.end())

namespace oni {
    namespace audio {
        AudioManager::AudioManager() {
            mMaxAudibleDistance = 150.f;

            FMOD::System *system{nullptr};

            auto result = FMOD::System_Create(&system);
            ERRCHECK(result);
            mSystem = std::unique_ptr<FMOD::System, FMODDeleter>(system, FMODDeleter());

            common::uint32 version;
            result = system->getVersion(&version);
            ERRCHECK(result);

            assert(version >= FMOD_VERSION);

            mMaxNumberOfChannels = 1024;

            result = system->init(mMaxNumberOfChannels, FMOD_INIT_NORMAL, nullptr);
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

            ROCKET = "resources/audio/rocket/2-idle-fast.wav";
            loadSound(ROCKET);

            loadSound("resources/audio/rocket/1-shot.wav");

            preLoadCollisionSoundEffects();
        }

        void
        AudioManager::tick(entities::EntityFactory &entityFactory,
                           const math::vec3 &playerPos) {
            mPlayerPos = playerPos;
            auto result = mSystem->update();
            ERRCHECK(result);

            // Engine
            {
                auto view = entityFactory.getEntityManager().createView<component::Tag_Audible,
                        component::Placement, component::Car, component::SoundTag>();
                for (auto &&entity : view) {
                    auto &car = view.get<component::Car>(entity);
                    auto &placement = view.get<component::Placement>(entity);
                    auto &soundTag = view.get<component::SoundTag>(entity);
                    math::vec3 velocity{car.velocity.x, car.velocity.y, 0.f};

                    // TODO: Using soundTag find the correct sound to play.
                    component::SoundID soundID = ENGINE_IDLE;

                    auto &entityChannel = getOrCreateLooping3DChannel(soundID, entity);
                    auto &entityPos = placement.position;
                    auto distance = (entityPos - mPlayerPos).len();
                    if (distance < mMaxAudibleDistance) {
                        auto pitch = static_cast< common::real32>(car.rpm) / 2000;
                        setPitch(*entityChannel.channel, pitch);
                        set3DPos(*entityChannel.channel, entityPos - mPlayerPos, velocity);

                        unPause(*entityChannel.channel);
                    } else {
                        pause(*entityChannel.channel);
                    }
                }
            }

            {
                auto view = entityFactory.getEntityManager().createView<component::Tag_Audible,
                        component::Placement, component::SoundTag>();
                for (auto &&entity : view) {
                    auto &pos = view.get<component::Placement>(entity).position;
                    auto &soundTag = view.get<component::SoundTag>(entity);

                    // TODO: Using soundTag find the correct sound to play.
                    if (soundTag != component::SoundTag::ROCKET) {
                        continue;
                    }
                    component::SoundID soundID = ROCKET;

                    auto &entityChannel = getOrCreateLooping3DChannel(soundID, entity);
                    auto distance = (pos - mPlayerPos).len();
                    if (distance < mMaxAudibleDistance) {

                        // TODO: Does it matter if this is accurate?
                        math::vec3 vel{1.f, 0.f, 0.f};
                        set3DPos(*entityChannel.channel, pos - mPlayerPos, vel);
                        unPause(*entityChannel.channel);
                    } else {
                        pause(*entityChannel.channel);
                    }
                }
            }
        }

        void
        AudioManager::playCollisionSoundEffect(component::EntityType A,
                                               component::EntityType B,
                                               const component::CollisionPos &pos) {
            auto soundID = createCollisionEffectID(A, B);
            assert(mCollisionEffects.find(soundID) != mCollisionEffects.end());
            auto distance = mPlayerPos - pos;
            // TODO: use ChannelGroup and use the volume from it
            common::real32 volume = 0.1f;
            common::real32 pitch = 1.f;
            playOneShot(mCollisionEffects[soundID], distance, volume, pitch);
            playOneShot(mCollisionEffects[soundID], distance, volume, pitch);
        }

        common::UInt16Pack
        AudioManager::createCollisionEffectID(component::EntityType A,
                                              component::EntityType B) {
            static_assert(sizeof(A) == sizeof(common::uint16), "Hashing will fail due to size mismatch");
            auto x = static_cast<common::uint16 >(A);
            auto y = static_cast<common::uint16 >(B);

            if (x > y) {
                std::swap(x, y); // Assuming soundEffect for A->B collision is same as B->A
            }

            auto soundID = math::packUInt16(x, y);
            return soundID;
        }

        void
        AudioManager::preLoadCollisionSoundEffects() {
            component::SoundID rocketWithUnknown = "resources/audio/collision/rocket-with-unknown.wav";
            loadSound(rocketWithUnknown);
            for (auto i = static_cast<common::uint16 >(component::EntityType::UNKNOWN);
                 i < static_cast<common::uint16>(component::EntityType::LAST);
                 ++i) {
                auto id = createCollisionEffectID(component::EntityType::SIMPLE_ROCKET,
                                                  static_cast<component::EntityType>(i));
                mCollisionEffects[id] = rocketWithUnknown;
            }
        }


        void
        AudioManager::kill(common::EntityID entityID) {
            for (auto it = mLooping3DChannels.begin(); it != mLooping3DChannels.end();) {
                if (it->second.entityID == entityID) {
                    auto result = it->second.channel->stop();
                    ERRCHECK(result);
                    it = mLooping3DChannels.erase(it);
                } else {
                    ++it;
                }
            }
        }

        void
        AudioManager::loadSound(const component::SoundID &id) {
            FMOD::Sound *sound{};
            auto result = mSystem->createSound(id.c_str(), FMOD_DEFAULT, nullptr, &sound);
            ERRCHECK(result);
            assert(sound);

            mSounds[id] = std::unique_ptr<FMOD::Sound, FMODDeleter>(sound, FMODDeleter());
        }

        FMOD::Channel *
        AudioManager::createChannel(const component::SoundID &id) {
            VALID(mSounds, id);

            FMOD::Channel *channel{nullptr};
            auto result = mSystem->playSound(mSounds[id].get(), nullptr, true, &channel);
            ERRCHECK(result);

            return channel;
        }

        void
        AudioManager::playOneShot(const component::SoundID &id,
                                  const math::vec3 &distance,
                                  common::real32 volume,
                                  common::real32 pitch) {
            VALID(mSounds, id);
            auto channel = createChannel(id);

            auto result = channel->setMode(FMOD_3D);
            ERRCHECK(result);

            setVolume(*channel, volume);

            setPitch(*channel, pitch);

            // TODO: Does this matter?
            math::vec3 velocity{1.f, 1.f, 0.f};
            set3DPos(*channel, distance, velocity);

            unPause(*channel);
        }

        static FMOD_RESULT
        endOfPlayCallback(FMOD_CHANNELCONTROL *channelControl,
                          FMOD_CHANNELCONTROL_TYPE type,
                          FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType,
                          void *,
                          void *) {
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
        }

        AudioManager::SoundEntityID
        AudioManager::getID(const component::SoundID &soundID,
                            common::EntityID entityID) {
            return soundID + std::to_string(entityID);
        }

        AudioManager::EntityChannel &
        AudioManager::getOrCreateLooping3DChannel(const component::SoundID &soundID,
                                                  common::EntityID entityID) {
            auto id = getID(soundID, entityID);
            if (mLooping3DChannels.find(id) == mLooping3DChannels.end()) {
                EntityChannel entityChannel;
                entityChannel.entityID = entityID;
                entityChannel.channel = createChannel(soundID);
                // TODO: This should not be happening, assign channel group and set volume on the group
                entityChannel.channel->setVolume(0.1f);
                entityChannel.channel->setMode(FMOD_LOOP_NORMAL | FMOD_3D);

                mLooping3DChannels[id] = entityChannel;
            }
            return mLooping3DChannels[id];
        }

        void
        AudioManager::setPitch(FMOD::Channel &channel,
                               common::real32 pitch) {
            if (pitch > 256) {
                pitch = 256;
            }
            auto result = channel.setPitch(pitch);
            ERRCHECK(result);
        }

        bool
        AudioManager::isPaused(FMOD::Channel &channel) {
            bool paused{false};
            auto result = channel.getPaused(&paused);
            ERRCHECK(result);
            return paused;
        }

        void
        AudioManager::unPause(FMOD::Channel &channel) {
            auto result = channel.setPaused(false);
            ERRCHECK(result);
        }

        void
        AudioManager::pause(FMOD::Channel &channel) {
            auto result = channel.setPaused(true);
            ERRCHECK(result);
        }

        void
        AudioManager::FMODDeleter::operator()(FMOD::Sound *s) const {
            s->release();
        }

        void
        AudioManager::FMODDeleter::operator()(FMOD::System *sys) const {
            sys->close();
            sys->release();
        }

        void
        AudioManager::set3DPos(FMOD::Channel &channel,
                               const math::vec3 &pos,
                               const math::vec3 &velocity) {
            FMOD_VECTOR fPos;
            fPos.x = pos.x;
            fPos.y = pos.y;
            fPos.z = pos.z;
            FMOD_VECTOR fVelocity;
            fVelocity.x = velocity.x;
            fVelocity.y = velocity.y;
            fVelocity.z = velocity.z;

            auto result = channel.set3DAttributes(&fPos, &fVelocity);
            ERRCHECK(result);

            result = channel.set3DMinMaxDistance(20.f, mMaxAudibleDistance); // In meters
            ERRCHECK(result);
        }

        void
        AudioManager::setVolume(FMOD::Channel &channel,
                                common::real32 volume) {
            auto result = channel.setVolume(volume);
            ERRCHECK(result);
        }
    }
}
