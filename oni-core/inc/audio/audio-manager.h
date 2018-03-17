#pragma once

#include <string>
#include <vector>

#include <soloud.h>
#include <soloud_wav.h>
#include <memory>

namespace oni {
    namespace audio {
        class AudioManager {
        public:
            AudioManager();

            ~AudioManager() = default;

            /**
             * @param name Path to the audio
             * @return unique ID
             */
            long loadSound(const std::string &name);

            /**
             * Unpause or play
             * @param soundID value returned by AudioManager::loadSound()
             */
            void playSound(long soundID);

        private:
            SoLoud::Soloud m_SoloudManager;
            std::vector<SoLoud::Wav> m_AudioSources;
            std::vector<std::string> m_AudioNames;
            std::vector<SoLoud::handle> m_Handles;

        };
    }
}