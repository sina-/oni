#pragma once

#include <string>
#include <vector>
#include <memory>

#include <fmod.hpp>

#include <oni-core/audio/audio-manager.h>

namespace oni {
    namespace audio {

        class AudioManagerFMOD : public AudioManager {
        public:
            AudioManagerFMOD();

            ~AudioManagerFMOD() override;

            void tick() override;

            oniSoundID loadSound(const std::string &name) override;

            void playSound(oniSoundID id) override;

            double pauseSound(oniSoundID id) override;

            void stopSound(oniSoundID id) override;

            void setLoop(oniSoundID id, bool loop) override;

            void setVolume(oniSoundID id, float volume) override;

            float getVolume(oniSoundID id) override;

            bool isPlaying(oniSoundID id) override;

            void seek(oniSoundID id, double position) override;

            void setPitch(oniSoundID id, float pitch) override;

        private:
            class FMODDeleter {
            public:
                void operator()(FMOD::Sound *s) const { s->release(); }

                void operator()(FMOD::System *sys) const {
                    sys->close();
                    sys->release();
                }
            };

        private:
            std::unique_ptr<FMOD::System, AudioManagerFMOD::FMODDeleter> mSystem;
            std::vector<std::unique_ptr<FMOD::Sound, AudioManagerFMOD::FMODDeleter>> mSounds;
            std::vector<std::unique_ptr<FMOD::Channel>> mChannels;

            // TODO: move it to const deceleration file, where ever that is
            const float ep = 0.00001f;
        };
    }
}