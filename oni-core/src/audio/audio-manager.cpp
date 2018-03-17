#include <audio/audio-manager.h>

#include <algorithm>

namespace oni {
    namespace audio {

        AudioManager::AudioManager() {
            using namespace SoLoud;
            m_SoloudManager = std::make_unique<SoLoud::Soloud>();
            m_SoloudManager->init(Soloud::FLAGS::CLIP_ROUNDOFF, Soloud::BACKENDS::SDL2, Soloud::AUTO, Soloud::AUTO);
        }

        long AudioManager::loadSound(const std::string &name) {
            // Avoid double loading
            auto it = std::find(m_AudioNames.begin(), m_AudioNames.end(), name);

            if (it == m_AudioNames.end()) {
                m_AudioNames.emplace_back(name);
                m_AudioSources.emplace_back(std::make_unique<SoLoud::Wav>());
                m_AudioSources.back()->load(name.c_str());

                // Get a handle to a paused audio. Use the handle to play the audio.
                auto handle = m_SoloudManager->play(*m_AudioSources.back(), 1, 0, true);
                m_Handles.push_back(handle);

                return m_Handles.size() - 1;
            } else {
                return it - m_AudioNames.begin();
            }

        }

        void AudioManager::playSound(long soundID) {
            if (m_SoloudManager->getPause(m_Handles[soundID])) {
                m_SoloudManager->setPause(m_Handles[soundID], false);
            } else {
                m_SoloudManager->play(*m_AudioSources[soundID]);
            }
        }

    }
}