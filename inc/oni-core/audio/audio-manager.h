#pragma once

#include <string>
#include <vector>
#include <memory>

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace audio {

        using common::oniSoundID;

        class AudioManager {
        public:
            AudioManager() = default;

            virtual ~AudioManager() = default;

            virtual void tick() = 0;

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
            virtual oni::common::real64 pauseSound(oniSoundID id) = 0;

            virtual void stopSound(oniSoundID id) = 0;

            virtual void setLoop(oniSoundID id, bool loop) = 0;

            virtual void setVolume(oniSoundID id, common::real32 volume) = 0;

            virtual common::real32 getVolume(oniSoundID id) = 0;

            virtual bool isPlaying(oniSoundID id) = 0;

            virtual void seek(oniSoundID id, oni::common::real64 position) = 0;

            virtual void setPitch(oniSoundID id, common::real32 pitch) = 0;
        };
    }
}
