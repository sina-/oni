#include <oni-core/audio/oni-audio-manager.h>

#include <assert.h>

#include <fmod.hpp>

#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/entities/oni-entities-factory.h>

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

            common::u32 version;
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

            mEngineIdleSound = component::SoundID{"resources/audio/car/car-2/idle-low-slow.wav"};
            loadSound(mEngineIdleSound);

/*            component::SoundID rocketFastSoundID = "resources/audio/rocket/1-fast.wav";
            loadSound(rocketFastSoundID);

            component::SoundID rocketSlowSoundID = "resources/audio/rocket/1-slow.wav";
            loadSound(rocketSlowSoundID);*/

            mRocketSound = component::SoundID{"resources/audio/rocket/2-idle-fast.wav"};
            loadSound(mRocketSound);

            auto shotSound = component::SoundID{"resources/audio/rocket/1-shot.wav"};
            loadSound(shotSound);

            preLoadCollisionSoundEffects();
        }

        void
        AudioManager::tick(entities::EntityFactory &entityFactory,
                           const component::WorldP3D &playerPos) {
            mPlayerPos = playerPos;
            auto result = mSystem->update();
            ERRCHECK(result);

            // Engine
            {
                auto view = entityFactory.getEntityManager().createView<component::Tag_Audible,
                        component::WorldP3D, component::Car, component::SoundTag>();
                for (auto &&entity : view) {
                    auto &car = view.get<component::Car>(entity);
                    auto &pos = view.get<component::WorldP3D>(entity);
                    auto &soundTag = view.get<component::SoundTag>(entity);
                    math::vec3 velocity{car.velocity.x, car.velocity.y, 0.f};

                    // TODO: Using soundTag find the correct sound to play.
                    auto &entityChannel = getOrCreateLooping3DChannel(mEngineIdleSound, entity);
                    auto distance = (pos.value - mPlayerPos.value).len();
                    if (distance < mMaxAudibleDistance) {
                        auto pitch = static_cast< common::r32>(car.rpm) / 2000;
                        setPitch(*entityChannel.channel, pitch);
                        set3DPos(*entityChannel.channel, pos.value - mPlayerPos.value, velocity);

                        unPause(*entityChannel.channel);
                    } else {
                        pause(*entityChannel.channel);
                    }
                }
            }

            {
                auto view = entityFactory.getEntityManager().createView<component::Tag_Audible,
                        component::WorldP3D, component::SoundTag>();
                for (auto &&entity : view) {
                    auto &pos = view.get<component::WorldP3D>(entity).value;
                    auto &soundTag = view.get<component::SoundTag>(entity);

                    // TODO: Using soundTag find the correct sound to play.
                    if (soundTag != component::SoundTag::ROCKET) {
                        continue;
                    }
                    auto &entityChannel = getOrCreateLooping3DChannel(mRocketSound, entity);
                    auto distance = (pos - mPlayerPos.value).len();
                    if (distance < mMaxAudibleDistance) {

                        // TODO: Does it matter if this is accurate?
                        math::vec3 vel{1.f, 0.f, 0.f};
                        set3DPos(*entityChannel.channel, pos - mPlayerPos.value, vel);
                        unPause(*entityChannel.channel);
                    } else {
                        pause(*entityChannel.channel);
                    }
                }
            }
        }

        void
        AudioManager::playCollisionSoundEffect(entities::EntityType A,
                                               entities::EntityType B,
                                               const component::WorldP3D &pos) {
            auto soundID = createCollisionEffectID(A, B);
            assert(mCollisionEffects.find(soundID) != mCollisionEffects.end());
            auto distance = mPlayerPos.value - pos.value;
            // TODO: use ChannelGroup and use the volume from it
            common::r32 volume = 0.1f;
            common::r32 pitch = 1.f;
            playOneShot(mCollisionEffects[soundID], distance, volume, pitch);
            playOneShot(mCollisionEffects[soundID], distance, volume, pitch);
        }

        common::u16p
        AudioManager::createCollisionEffectID(entities::EntityType A,
                                              entities::EntityType B) {
            static_assert(sizeof(A) == sizeof(common::u16), "Hashing will fail due to size mismatch");
            auto x = static_cast<common::u16 >(A);
            auto y = static_cast<common::u16 >(B);

            if (x > y) {
                std::swap(x, y); // Assuming soundEffect for A->B collision is same as B->A
            }

            auto soundID = math::pack_u16(x, y);
            return soundID;
        }

        void
        AudioManager::preLoadCollisionSoundEffects() {
            auto rocketWithUnknown = component::SoundID{"resources/audio/collision/rocket-with-unknown.wav"};
            loadSound(rocketWithUnknown);
            for (auto i = static_cast<common::u16 >(entities::EntityType::UNKNOWN);
                 i < static_cast<common::u16>(entities::EntityType::LAST);
                 ++i) {
                auto id = createCollisionEffectID(entities::EntityType::SIMPLE_ROCKET,
                                                  static_cast<entities::EntityType>(i));
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
            auto result = mSystem->createSound(id.value.data(), FMOD_DEFAULT, nullptr, &sound);
            ERRCHECK(result);
            assert(sound);

            std::unique_ptr<FMOD::Sound, FMODDeleter> value(sound);
            mSounds.insert({id, std::move(value)});
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
                                  common::r32 volume,
                                  common::r32 pitch) {
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
        AudioManager::createNewID(const component::SoundID &soundID,
                                  common::EntityID entityID) {
            return std::string(soundID.value) + std::to_string(entityID);
        }

        AudioManager::EntityChannel &
        AudioManager::getOrCreateLooping3DChannel(const component::SoundID &soundID,
                                                  common::EntityID entityID) {
            auto id = createNewID(soundID, entityID);
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
                               common::r32 pitch) {
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
                                common::r32 volume) {
            auto result = channel.setVolume(volume);
            ERRCHECK(result);
        }
    }
}
