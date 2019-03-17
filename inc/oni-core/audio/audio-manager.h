#pragma once

#include <string>
#include <unordered_map>

#include <oni-core/common/typedefs.h>
#include <oni-core/component/entity-definition.h>
#include <oni-core/component/audio.h>

namespace oni {
    namespace audio {
        class AudioManager {
        public:
            AudioManager();

            virtual ~AudioManager() = default;

            virtual void tick() = 0;

            virtual common::SoundID loadSound(const std::string &name) = 0;

            virtual common::SoundID loadLoopingSound(const std::string &name) = 0;

            virtual void playLoopingSound(common::SoundID) = 0;

            virtual void playSoundOnce(common::SoundID) = 0;

            virtual oni::common::real64 pauseSound(common::SoundID) = 0;

            virtual void stopSound(common::SoundID) = 0;

            virtual void setLoop(common::SoundID, bool loop) = 0;

            virtual void setVolume(common::SoundID, common::real32 volume) = 0;

            virtual bool isPlaying(common::SoundID) = 0;

            virtual void seek(common::SoundID, oni::common::real64 position) = 0;

            virtual void setPitch(common::SoundID, common::real32 pitch) = 0;

        public:
            void collisionSoundEffect(component::EntityType, component::EntityType);

            void preLoadCollisionSoundEffects();

        protected:
            std::unordered_map<common::UInt16Pack, common::SoundID> mCollisionEffects;
            std::unordered_map<component::SoundEffectID, common::SoundID> mSoundEffects;

        private:
            common::UInt16Pack createID(component::EntityType, component::EntityType);
        };
    }
}
