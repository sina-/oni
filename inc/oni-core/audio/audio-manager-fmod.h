#pragma once

#include <string>
#include <vector>
#include <memory>

#include <oni-core/audio/audio-manager.h>
#include <oni-core/common/typedefs.h>

namespace FMOD {
    class Sound;

    class System;

    class Channel;
}

namespace oni {
    namespace audio {
        using common::oniSoundID;

        class FMODDeleter {
        public:
            void operator()(FMOD::Sound *s) const;

            void operator()(FMOD::System *sys) const;
        };

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

        private:
            std::unique_ptr<FMOD::System, FMODDeleter> mSystem;
            std::vector<std::unique_ptr<FMOD::Sound, FMODDeleter>> mSounds;
            std::vector<std::unique_ptr<FMOD::Channel>> mChannels;
        };
    }
}