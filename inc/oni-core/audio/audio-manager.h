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

            void playCollisionSoundEffect(component::EntityType, component::EntityType);

            void kill(common::EntityID);

        private:
            void loadSound(const component::SoundID &);

            void play(const component::SoundID &id, const math::vec2 &distance, common::real32 volume,
                      common::real32 pitch);

            void tryPlay(const component::SoundID &, common::EntityID);

            void kill(const component::SoundID &id, common::EntityID entityID);


            void fadeOut(const component::SoundID &id);

            void fadeOut(const component::SoundID &id, common::EntityID entityID);


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
            void preLoadCollisionSoundEffects();

            common::UInt16Pack createCollisionEffectID(component::EntityType, component::EntityType);

            FMOD::Channel *createChannel(const component::SoundID &);

            SoundEntityID getID(const component::SoundID &, common::EntityID);

            EntityChannel &getOrCreateChannelIfMissing(const component::SoundID &, common::EntityID);

            void setPitch(EntityChannel &, common::real32 pitch);

            void set3DPos(EntityChannel &, const math::vec3 &pos, const math::vec3 &velocity);

            bool isPaused(EntityChannel &);

            void unPause(EntityChannel &);

            void pause(EntityChannel &);

        private:
            std::unique_ptr<FMOD::System, FMODDeleter> mSystem;
            std::unordered_map<component::SoundID, std::unique_ptr<FMOD::Sound, FMODDeleter>> mSounds;

            std::map<SoundEntityID, EntityChannel> mEntityAudioChannel;
            component::SoundID mEngineIdle;
            component::SoundID ENGINE_IDLE;
            component::SoundID ROCKET;

            common::real32 mMaxAuidbleDistance{0.f};

            std::unordered_map<common::UInt16Pack, component::SoundID> mCollisionEffects;
        };
    }
}