#pragma once

#include <string>
#include <unordered_map>

#include <oni-core/common/typedefs.h>
#include <oni-core/component/entity-definition.h>

namespace oni {
    namespace audio {
        class AudioManager {
        public:
            AudioManager();

            virtual ~AudioManager() = default;

            virtual void tick() = 0;

            virtual common::OniSoundID loadSound(const std::string &name) = 0;

            virtual void playSound(common::OniSoundID) = 0;

            virtual oni::common::real64 pauseSound(common::OniSoundID) = 0;

            virtual void stopSound(common::OniSoundID) = 0;

            virtual void setLoop(common::OniSoundID, bool loop) = 0;

            virtual void setVolume(common::OniSoundID, common::real32 volume) = 0;

            virtual common::real32 getVolume(common::OniSoundID) = 0;

            virtual bool isPlaying(common::OniSoundID) = 0;

            virtual void seek(common::OniSoundID, oni::common::real64 position) = 0;

            virtual void setPitch(common::OniSoundID, common::real32 pitch) = 0;

        public:
            void collisionSoundEffect(component::EntityType, component::EntityType);

            void preLoadSound();

        protected:
            std::unordered_map<common::UInt16Pack, common::OniSoundID> mCollisionEffects;

        private:
            common::UInt16Pack findID(component::EntityType, component::EntityType);
        };
    }
}
