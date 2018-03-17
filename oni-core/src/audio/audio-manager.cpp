#include <audio/audio-manager.h>

#include <algorithm>

namespace oni {
    namespace audio {

        AudioManager::AudioManager() {
            using namespace SoLoud;
            m_SoloudManager = SoLoud::Soloud();
            m_SoloudManager.init(Soloud::FLAGS::CLIP_ROUNDOFF, Soloud::BACKENDS::SDL2, Soloud::AUTO, Soloud::AUTO);
        }

        long AudioManager::loadSound(const std::string &name) {
            // Avoid double loading
            auto it = std::find(m_AudioNames.begin(), m_AudioNames.end(), name);

            if (it == m_AudioNames.end()) {
                m_AudioNames.emplace_back(name);
                m_AudioSources.emplace_back(SoLoud::Wav());
                m_AudioSources.back().load(name.c_str());

                // Get a handle to a paused audio. Use the handle to play the audio.
                auto handle = m_SoloudManager.play(m_AudioSources.back(), 1, 0, true);
                m_Handles.push_back(handle);

                return m_Handles.size() - 1;
            } else {
                return it - m_AudioNames.begin();
            }
        }

        void AudioManager::playSound(long soundID) {
            if (m_SoloudManager.getPause(m_Handles[soundID])) {
                m_SoloudManager.setPause(m_Handles[soundID], false);
            } else {
                // NOTE: if play is called on paused source, none of the attribute settings will work.
                m_SoloudManager.play(m_AudioSources[soundID]);
            }
        }

        void AudioManager::pauseSound(long soundID) {
            m_SoloudManager.setPause(m_Handles[soundID], true);
        }

        void AudioManager::setLoop(long soundID, bool loop) {
            m_SoloudManager.setLooping(m_Handles[soundID], loop);
        }

        void AudioManager::stopSound(long soundID) {
            m_SoloudManager.stop(m_Handles[soundID]);
        }

        void AudioManager::setVolume(long soundID, float volume) {
            m_SoloudManager.setVolume(m_Handles[soundID], volume);
        }

        float AudioManager::getVolume(long soundID) {
            m_SoloudManager.getVolume(m_Handles[soundID]);
        }


    }
}