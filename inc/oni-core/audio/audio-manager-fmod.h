#pragma once

#include <string>
#include <vector>
#include <memory>

#include <oni-core/audio/audio-manager.h>

namespace oni {
    namespace audio {
        typedef long oniSoundID;

        class AudioManagerFmod : public AudioManager {
        public:
            AudioManagerFmod();

            ~AudioManagerFmod();

            oniSoundID loadSound(const std::string &name) override;

            void playSound(oniSoundID id) override;

            double pauseSound(oniSoundID id) override;

            void stopSound(oniSoundID id) override;

            void setLoop(oniSoundID id, bool loop) override;

            void setVolume(oniSoundID id, float volume) override;

            float getVolume(oniSoundID id) override;

            bool isPlaying(oniSoundID id) override;

            void seek(oniSoundID id, double position) override;
        };
    }
}