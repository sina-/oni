#pragma once

#include <string>
#include <vector>
#include <memory>

#include <soloud.h>
#include <soloud_wav.h>

#include <oni-core/audio/audio-manager.h>

namespace oni {
    namespace audio {
        typedef long oniSoundID;

        class AudioManagerSoloud : public AudioManager {
        public:
            AudioManagerSoloud();

            ~AudioManagerSoloud();

            oniSoundID loadSound(const std::string &name) override;

            void playSound(oniSoundID id) override;

            double pauseSound(oniSoundID id) override;

            void stopSound(oniSoundID id) override;

            void setLoop(oniSoundID id, bool loop) override;

            void setVolume(oniSoundID id, float volume) override;

            float getVolume(oniSoundID id) override;

            bool isPlaying(oniSoundID id) override;

            void seek(oniSoundID id, double position) override;

        private:
            SoLoud::Soloud mSoloudManager;
            // TODO: Having this on the heap beats the whole purpose of memory alignment, but I couldn't figure
            // out how to allocate it on the stack. On resizing of std::vector, the program crashes with
            // double de-allocation error. One work around is to reserve as big of a vector as there are
            // wave files in the game, but that would be wasteful memory. Otherwise, I have to find the cause of
            // double de-allocation
            std::vector<std::unique_ptr<SoLoud::Wav>> mAudioSources;
            std::vector<std::string> mAudioNames;
            std::vector<SoLoud::handle> mHandles;
        };
    }
}
