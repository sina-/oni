#pragma once

#include <vector>
#include <memory>
#include <map>
#include <unordered_map>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/component/oni-component-audio.h>
#include <oni-core/entities/oni-entities-entity.h>
#include <oni-core/component/oni-component-physics.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/asset/oni-asset-manager.h>

namespace FMOD {
    class Sound;

    class System;

    class Channel;

    class ChannelGroup;
}

namespace oni {
    namespace asset {
        class AssetManager;
    }
    namespace entities {
        class EntityManager;
    }

    namespace math {
        struct vec3;
    }
    namespace audio {
        class AudioManager {
        public:
            explicit AudioManager(asset::AssetManager &);

            void
            tick(entities::EntityManager &,
                 const component::WorldP3D &playerPos);

            void
            playCollisionSoundEffect(entities::EntityType A,
                                     entities::EntityType B,
                                     const component::WorldP3D &pos);

            void
            kill(common::EntityID);

            void
            playOneShot(const component::Sound &,
                        const math::vec3 &distance);

            void
            setChannelGroupVolume(component::ChannelGroup,
                                  common::r32 volume);

        private:
            class FMODDeleter {
            public:
                void
                operator()(FMOD::Sound *s) const;

                void
                operator()(FMOD::System *sys) const;

                void
                operator()(FMOD::ChannelGroup *channel) const;
            };

            using CollisionSoundTag = common::u16p;
            using EntitySoundTag = common::u32p;

            struct EntityChannel {
                common::EntityID entityID{0};
                FMOD::Channel *channel{nullptr};
            };

        private:
            void
            preLoadSounds();

            static CollisionSoundTag
            createCollisionEffectID(entities::EntityType,
                                    entities::EntityType);

            FMOD::Channel *
            createChannel(const component::Sound &);

            void
            loadSound(component::SoundTag,
                      std::string_view);

            static EntitySoundTag
            createEntitySoundID(component::SoundTag,
                                common::EntityID);

            AudioManager::EntityChannel &
            getOrCreateLooping3DChannel(const component::Sound &,
                                        common::EntityID);

            void
            setPitch(FMOD::Channel &,
                     common::r32 pitch);

            void
            set3DPos(FMOD::Channel &,
                     const math::vec3 &pos,
                     const math::vec3 &velocity);

            bool
            isPaused(FMOD::Channel &);

            void
            unPause(FMOD::Channel &);

            void
            pause(FMOD::Channel &);

            void
            setVolume(FMOD::Channel &,
                      common::r32 volume);

        private:
            asset::AssetManager &mAssetManager;
            std::unique_ptr<FMOD::System, FMODDeleter> mSystem;

            std::unordered_map<component::ChannelGroup, std::unique_ptr<FMOD::ChannelGroup, FMODDeleter>> mChannelGroups;
            std::unordered_map<component::SoundTag, std::unique_ptr<FMOD::Sound, FMODDeleter>> mSounds;
            std::unordered_map<AudioManager::EntitySoundTag, EntityChannel> mLoopingChannels;
            std::unordered_map<AudioManager::CollisionSoundTag, component::SoundTag> mCollisionEffects;

            component::WorldP3D mPlayerPos{};

            common::r32 mMaxAudibleDistance{0.f};
            common::i32 mMaxNumberOfChannels{0};
        };
    }
}