#pragma once

#include <vector>
#include <memory>
#include <map>

#include <oni-core/common/typedefs.h>
#include <oni-core/entities/entity-factory.h>

namespace FMOD {
    class Sound;

    class System;

    class Channel;
}

namespace oni {
    namespace audio {
        class AudioManager {
        public:
            AudioManager();

            void tick(entities::EntityFactory &, const math::vec3 &playerPos);

            void
            playCollisionSoundEffect(component::EntityType, component::EntityType, const component::CollisionPos &);

            void kill(common::EntityID);

            void playOneShot(const component::SoundID &id, const math::vec3 &distance, common::real32 volume,
                             common::real32 pitch);

        private:
            void loadSound(const component::SoundID &);

        private:
            class FMODDeleter {
            public:
                void operator()(FMOD::Sound *s) const;

                void operator()(FMOD::System *sys) const;
            };

            using SoundEntityID = std::string;

            struct EntityChannel {
                common::EntityID entityID{0};
                FMOD::Channel *channel{nullptr};
            };

        private:
            common::UInt16Pack createCollisionEffectID(component::EntityType, component::EntityType);

            FMOD::Channel *createChannel(const component::SoundID &);

            SoundEntityID getID(const component::SoundID &, common::EntityID);

            EntityChannel &getOrCreateLooping3DChannel(const component::SoundID &soundID, common::EntityID entityID);

            void preLoadCollisionSoundEffects();

            void setPitch(FMOD::Channel &, common::real32 pitch);

            void set3DPos(FMOD::Channel &, const math::vec3 &pos, const math::vec3 &velocity);

            bool isPaused(FMOD::Channel &);

            void unPause(FMOD::Channel &);

            void pause(FMOD::Channel &);

            void setVolume(FMOD::Channel &, common::real32 volume);

        private:
            std::unique_ptr<FMOD::System, FMODDeleter> mSystem;
            std::unordered_map<component::SoundID, std::unique_ptr<FMOD::Sound, FMODDeleter>> mSounds;

            std::map<SoundEntityID, EntityChannel> mLooping3DChannels;

            std::unordered_map<common::UInt16Pack, component::SoundID> mCollisionEffects;

            math::vec3 mPlayerPos{};

            common::real32 mMaxAudibleDistance{0.f};
            common::int32 mMaxNumberOfChannels{0};

        private:
            component::SoundID ENGINE_IDLE;
            component::SoundID ROCKET;
        };
    }
}