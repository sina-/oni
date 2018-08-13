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

            OniSoundID loadSound(const std::string &name) override;

            void playSound(OniSoundID id) override;

            oni::common::real64 pauseSound(OniSoundID id) override;

            void stopSound(OniSoundID id) override;

            void setLoop(OniSoundID id, bool loop) override;

            void setVolume(OniSoundID id, common::real32 volume) override;

            common::real32 getVolume(OniSoundID id) override;

            bool isPlaying(OniSoundID id) override;

            void seek(OniSoundID id, oni::common::real64 position) override;

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
