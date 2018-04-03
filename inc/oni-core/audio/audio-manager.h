#pragma once

#include <string>
#include <vector>
#include <memory>

namespace oni {
    namespace audio {
        typedef long oniSoundID;

        class AudioManager {
        public:
            AudioManager() = default;

            /**
             * @param name Path to the audio
             * @return unique ID
             */
            virtual oniSoundID loadSound(const std::string &name) = 0;

            /**
             * Unpause or play
             * @param id value returned by AudioManager::loadSound()
             */
            virtual void playSound(oniSoundID id) = 0;

            /**
             * Sets the audio to paused and returns played duration
             * @param id
             * @return duration played
             */
            virtual double pauseSound(oniSoundID id) = 0;

            virtual void stopSound(oniSoundID id) = 0;

            virtual void setLoop(oniSoundID id, bool loop) = 0;

            virtual void setVolume(oniSoundID id, float volume) = 0;

            virtual float getVolume(oniSoundID id) = 0;

            virtual bool isPlaying(oniSoundID id) = 0;

            virtual void seek(oniSoundID id, double position) = 0;
        };
    }
}
