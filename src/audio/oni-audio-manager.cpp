#include <oni-core/audio/oni-audio-manager.h>

#include <assert.h>

#include <fmod.hpp>

#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/entities/oni-entities-manager.h>

#define ERRCHECK(_result) assert((_result) == FMOD_OK)

namespace oni {
    namespace audio {
        AudioManager::AudioManager(asset::AssetManager &assetManager) : mAssetManager(assetManager) {
            mMaxAudibleDistance = 150.f;
            mMaxNumberOfChannels = 1024;

            FMOD::System *system{nullptr};
            auto result = FMOD::System_Create(&system);
            ERRCHECK(result);
            mSystem = std::unique_ptr<FMOD::System, FMODDeleter>(system, FMODDeleter());

            common::u32 version;
            result = mSystem->getVersion(&version);
            ERRCHECK(result);

            assert(version >= FMOD_VERSION);

            result = mSystem->init(mMaxNumberOfChannels, FMOD_INIT_NORMAL, nullptr);
            ERRCHECK(result);

            result = mSystem->update();
            ERRCHECK(result);

/*            component::SoundID backgroundSoundID = "resources/audio/beat.wav";
            loadSound(backgroundSoundID);
            attachControls(backgroundSoundID);
            setLoop(backgroundSoundID, true);
            setVolume(backgroundSoundID, 0.2f);
            //playSound(backgroundSoundID);*/

            FMOD::ChannelGroup *group{nullptr};
            result = mSystem->createChannelGroup("effectsChannel", &group);
            ERRCHECK(result);
            auto effectsGroup = std::unique_ptr<FMOD::ChannelGroup, FMODDeleter>(group, FMODDeleter());
            effectsGroup->setVolume(1.f);
            mChannelGroups[component::ChannelGroup::EFFECT] = std::move(effectsGroup);
            group = nullptr;

            result = mSystem->createChannelGroup("musicChannel", &group);
            ERRCHECK(result);
            auto musicGroup = std::unique_ptr<FMOD::ChannelGroup, FMODDeleter>(group, FMODDeleter());
            musicGroup->setVolume(1.f);
            mChannelGroups[component::ChannelGroup::MUSIC] = std::move(musicGroup);
            group = nullptr;

            preLoadSounds();
        }

        void
        AudioManager::tick(entities::EntityManager &manager,
                           const component::WorldP3D &playerPos) {
            mPlayerPos = playerPos;
            auto result = mSystem->update();
            ERRCHECK(result);

            /// Engine pitch
            {
                auto view = manager.createView<
                        component::Tag_Audible,
                        component::Car,
                        component::Sound>();
                for (auto &&entity : view) {
                    auto &car = view.get<component::Car>(entity);
                    auto &sound = view.get<component::Sound>(entity);

                    // TODO: This needs to go into client side entity manager as a complementary entity
                    sound.pitch = static_cast< common::r32>(car.rpm) / 2000;
                }
            }

            /// Play and Pause
            {
                auto view = manager.createView<
                        component::Tag_Audible,
                        component::WorldP3D,
                        component::Sound>();
                for (auto &&entity : view) {
                    auto &pos = view.get<component::WorldP3D>(entity).value;
                    auto &sound = view.get<component::Sound>(entity);

                    auto &entityChannel = getOrCreateLooping3DChannel(sound, entity);
                    auto distance = (pos - mPlayerPos.value).len();
                    if (distance < mMaxAudibleDistance) {

                        // TODO: Does it matter if this is accurate?
                        math::vec3 vel{1.f, 0.f, 0.f};
                        set3DPos(*entityChannel.channel, pos - mPlayerPos.value, vel);
                        setPitch(*entityChannel.channel, sound.pitch);
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
            auto collisionTag = createCollisionEffectID(A, B);
            auto distance = mPlayerPos.value - pos.value;
            auto soundTag = mCollisionEffects[collisionTag];
            assert(soundTag);
            auto sound = component::Sound{soundTag, component::ChannelGroup::EFFECT, 1.f};
            playOneShot(sound, distance);
        }

        AudioManager::CollisionSoundTag
        AudioManager::createCollisionEffectID(entities::EntityType A,
                                              entities::EntityType B) {
            static_assert(sizeof(A) == sizeof(common::u16), "Hashing will fail due to size mismatch");
            auto x = math::enumCast(A);
            auto y = math::enumCast(B);

            if (x > y) {
                std::swap(x, y); // Assuming soundEffect for A->B collision is same as B->A
            }

            auto soundID = math::pack_u16(x, y);
            return soundID;
        }

        void
        AudioManager::preLoadSounds() {
            for (auto i = math::enumCast(component::SoundTag::UNKNOWN) + 1;
                 i < math::enumCast(component::SoundTag::LAST);
                 ++i) {
                auto tag = static_cast<component::SoundTag>(i);
                auto path = mAssetManager.getAssetFilePath(tag);
                loadSound(tag, path);
            }

            for (auto i = math::enumCast(entities::EntityType::UNKNOWN);
                 i < math::enumCast(entities::EntityType::LAST);
                 ++i) {
                auto id = createCollisionEffectID(entities::EntityType::SIMPLE_ROCKET,
                                                  static_cast<entities::EntityType>(i));
                mCollisionEffects[id] = component::SoundTag::COLLISION_ROCKET_UNKNOWN;
            }
        }

        void
        AudioManager::kill(common::EntityID entityID) {
            for (auto it = mLoopingChannels.begin(); it != mLoopingChannels.end();) {
                if (it->second.entityID == entityID) {
                    auto result = it->second.channel->stop();
                    ERRCHECK(result);
                    it = mLoopingChannels.erase(it);
                } else {
                    ++it;
                }
            }
        }

        void
        AudioManager::loadSound(component::SoundTag tag,
                                std::string_view filePath) {
            FMOD::Sound *sound{};
            auto result = mSystem->createSound(filePath.data(), FMOD_DEFAULT, nullptr, &sound);
            ERRCHECK(result);
            assert(sound);

            std::unique_ptr<FMOD::Sound, FMODDeleter> value(sound);
            mSounds.insert({tag, std::move(value)});
        }

        FMOD::Channel *
        AudioManager::createChannel(const component::Sound &sound) {
            assert(mChannelGroups[sound.group]);
            auto *group = mChannelGroups[sound.group].get();
            assert(group);
            assert(mSounds[sound.tag]);

            FMOD::Channel *channel{nullptr};
            auto paused = true;
            auto result = mSystem->playSound(mSounds[sound.tag].get(), group, paused, &channel);
            ERRCHECK(result);

            return channel;
        }

        void
        AudioManager::playOneShot(const component::Sound &sound,
                                  const math::vec3 &distance) {
            auto *channel = createChannel(sound);
            assert(channel);

            auto result = channel->setMode(FMOD_3D);
            ERRCHECK(result);

            setPitch(*channel, sound.pitch);

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

        AudioManager::EntitySoundTag
        AudioManager::createEntitySoundID(component::SoundTag tag,
                                          common::EntityID id) {
            auto result = math::pack_u32(math::enumCast(tag), id);
            return result;
        }

        AudioManager::EntityChannel &
        AudioManager::getOrCreateLooping3DChannel(const component::Sound &sound,
                                                  common::EntityID entityID) {
            auto id = createEntitySoundID(sound.tag, entityID);
            if (mLoopingChannels.find(id) == mLoopingChannels.end()) {
                EntityChannel entityChannel;
                entityChannel.entityID = entityID;
                entityChannel.channel = createChannel(sound);
                entityChannel.channel->setMode(FMOD_LOOP_NORMAL | FMOD_3D);

                mLoopingChannels[id] = entityChannel;
            }
            return mLoopingChannels[id];
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
        AudioManager::FMODDeleter::operator()(FMOD::ChannelGroup *channel) const {
            channel->stop();
            channel->release();
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

        void
        AudioManager::setChannelGroupVolume(component::ChannelGroup channelGroup,
                                            common::r32 volume) {
            assert(mChannelGroups[channelGroup]);
            auto *group = mChannelGroups[channelGroup].get();
            assert(group);
            group->setVolume(volume);
        }
    }
}
