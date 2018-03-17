#pragma once

#include <string>
#include <vector>

#include <gorilla/ga.h>
#include <gorilla/gau.h>

namespace oni {
    namespace audio {
        class AudioManager {
        public:
            AudioManager();

            ~AudioManager();

            unsigned int loadSound(const std::string &name);

            void playSound(unsigned int soundID);

        private:
            gau_Manager *m_GAManager;
            ga_Mixer *m_GAMixer;

            std::vector<ga_Sound *> m_GASound;
            std::vector<ga_Handle *> m_GAHandle;
            std::vector<char> m_FinishedPlaying;

            // Next available handle ID. It can be used to index into m_GAHandle.
            unsigned int soundID;
            std::vector<std::string> m_LoadedSounds;

        private:
            static void playFinished(ga_Handle * in_handle, void* in_context);
        };
    }
}