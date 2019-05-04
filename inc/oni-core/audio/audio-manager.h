#pragma once

#include <vector>
#include <memory>
#include <map>
#include <unordered_map>

#include <oni-core/common/typedefs.h>
#include <oni-core/component/audio.h>
#include <oni-core/entities/entity.h>
#include <oni-core/component/physic.h>

namespace FMOD {
    class Sound;

    class System;

    class Channel;
}

namespace oni {
    namespace entities {
        class EntityFactory;
    }

    namespace math {
        struct vec3;
    }
    namespace audio {
        class AudioManager {
        public:
            AudioManager();

            void
            tick(entities::EntityFactory &,
                 const math::vec3 &playerPos);

            void
            playCollisionSoundEffect(entities::EntityType,
                                     entities::EntityType,
                                     const component::CollisionPos &);

            void
            kill(common::EntityID);

            void
            playOneShot(const component::SoundID &id,
                        const math::vec3 &distance,
                        common::real32 volume,
                        common::real32 pitch);

        private:
            class FMODDeleter {
            public:
                void
                operator()(FMOD::Sound *s) const;

                void
                operator()(FMOD::System *sys) const;
            };

            using SoundEntityID = std::string;

            struct EntityChannel {
                common::EntityID entityID{0};
                FMOD::Channel *channel{nullptr};
            };

            struct SoundIDHash {
                std::size_t
                operator()(const component::SoundID &soundID) const noexcept {
                    std::hash<std::string> func{};
                    return func(soundID.value);
                }
            };

            struct SoundIDCompare {
                bool
                operator()(const component::SoundID &left,
                           const component::SoundID &right) const {
                    return left.value == right.value;
                }
            };

        private:
            common::UInt16Pack
            createCollisionEffectID(entities::EntityType,
                                    entities::EntityType);

            FMOD::Channel *
            createChannel(const component::SoundID &);

            void
            loadSound(const component::SoundID &);

            SoundEntityID
            createNewID(const component::SoundID &soundID,
                        common::EntityID entityID);

            EntityChannel &
            getOrCreateLooping3DChannel(const component::SoundID &soundID,
                                        common::EntityID entityID);

            void
            preLoadCollisionSoundEffects();

            void
            setPitch(FMOD::Channel &,
                     common::real32 pitch);

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
                      common::real32 volume);

        private:
            std::unique_ptr<FMOD::System, FMODDeleter> mSystem;
            std::unordered_map<component::SoundID, std::unique_ptr<FMOD::Sound, FMODDeleter>, AudioManager::SoundIDHash, AudioManager::SoundIDCompare> mSounds;

            std::map<SoundEntityID, EntityChannel> mLooping3DChannels;

            std::unordered_map<common::UInt16Pack, component::SoundID> mCollisionEffects;

            math::vec3 mPlayerPos{};

            common::real32 mMaxAudibleDistance{0.f};
            common::int32 mMaxNumberOfChannels{0};

        private:
            component::SoundID mEngineIdleSound;
            component::SoundID mRocketSound;
        };
    }
}