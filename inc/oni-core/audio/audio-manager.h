#pragma once

#include <string>
#include <unordered_map>

#include <oni-core/common/typedefs.h>
#include <oni-core/component/entity-definition.h>
#include <oni-core/component/audio.h>

namespace oni {
    namespace entities {
        class EntityFactory;
    }

    namespace audio {
        class AudioManager {
        public:
            AudioManager();

            virtual ~AudioManager() = default;

            virtual void tick(entities::EntityFactory &) = 0;

            virtual void loadSound(const component::SoundID &) = 0;

            virtual void attachControls(const component::SoundID &) = 0;

            virtual void play(const component::SoundID &) = 0;

            virtual void setLoop(const component::SoundID &, bool loop) = 0;

            virtual void setPitch(const component::SoundID &, common::real32 pitch) = 0;

            virtual common::real64 pauseSound(const component::SoundID &) = 0;

            virtual void fadeOut(const component::SoundID &) = 0;

            virtual void setVolume(const component::SoundID &, common::real32 volume) = 0;

            virtual bool isPlaying(const component::SoundID &) = 0;

            virtual void seek(const component::SoundID &, common::real64 position) = 0;

        public:
            void playCollisionSoundEffect(component::EntityType, component::EntityType);

            void preLoadCollisionSoundEffects();

        protected:
            std::unordered_map<common::UInt16Pack, component::SoundID> mCollisionEffects;

        private:
            common::UInt16Pack createCollisionEffectID(component::EntityType, component::EntityType);
        };
    }
}
