#include <audio/audio-manager.h>

#include <algorithm>

#include <utils/oni-assert.h>
#include <thread>

namespace oni {
    namespace audio {

        AudioManager::AudioManager() {
            gc_initialize(nullptr);
            m_GAManager = gau_manager_create();
            m_GAMixer = gau_manager_mixer(m_GAManager);

            soundID = 0;
        }

        AudioManager::~AudioManager() {
            for (auto sound: m_GASound) {
                ga_sound_release(sound);
            }

            gau_manager_destroy(m_GAManager);
            gc_shutdown();
        }

        unsigned int AudioManager::loadSound(const std::string &name) {
            // Avoid double loading of sounds
            ONI_DEBUG_ASSERT(std::find(m_LoadedSounds.begin(), m_LoadedSounds.end(), name) == m_LoadedSounds.end());

            unsigned int newSoundID = soundID;
            m_GASound.emplace_back(gau_load_sound_file(name.c_str(), "wav"));
            m_FinishedPlaying.emplace_back(0);
            m_GAHandle.emplace_back(
                    gau_create_handle_sound(m_GAMixer, m_GASound.back(), &playFinished, &m_FinishedPlaying.back(),
                                            nullptr));

            m_LoadedSounds.emplace_back(name);
            soundID++;

            return newSoundID;
        }

        void AudioManager::playSound(unsigned int soundID) {
            new std::thread([this, soundID]() {
                ga_handle_play(m_GAHandle[soundID]);
                while (!m_FinishedPlaying[soundID]) {
                    gau_manager_update(m_GAManager);
                    //gc_thread_sleep(1);
                }
            });
        }

        void AudioManager::playFinished(ga_Handle *in_handle, void *in_context) {
            auto finished = (char *) (in_context);
            *finished = 1;
        }
    }
}