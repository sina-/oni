#pragma once

#include <string>

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace audio {

        using common::OniSoundID;

        class AudioManager {
        public:
            AudioManager() = default;

            virtual ~AudioManager() = default;

            virtual void tick() = 0;

            /**
             * @param name Path to the audio
             * @return unique ID
             */
            virtual OniSoundID loadSound(const std::string &name) = 0;

            /**
             * Unpause or play
             * @param id value returned by AudioManager::loadSound()
             */
            virtual void playSound(OniSoundID id) = 0;

            /**
             * Sets the audio to paused and returns played duration
             * @param id
             * @return duration played
             */
            virtual oni::common::real64 pauseSound(OniSoundID id) = 0;

            virtual void stopSound(OniSoundID id) = 0;

            virtual void setLoop(OniSoundID id, bool loop) = 0;

            virtual void setVolume(OniSoundID id, common::real32 volume) = 0;

            virtual common::real32 getVolume(OniSoundID id) = 0;

            virtual bool isPlaying(OniSoundID id) = 0;

            virtual void seek(OniSoundID id, oni::common::real64 position) = 0;

            virtual void setPitch(OniSoundID id, common::real32 pitch) = 0;
        };
    }
}
