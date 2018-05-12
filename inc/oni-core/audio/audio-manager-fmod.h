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

            void operator()(FMOD::Channel *channel) const;
        };

        class AudioManagerFMOD : public AudioManager {
        public:
            AudioManagerFMOD();

            void tick() override;

            oniSoundID loadSound(const std::string &name) override;

            void playSound(oniSoundID id) override;

            oni::common::real64 pauseSound(oniSoundID id) override;

            void stopSound(oniSoundID id) override;

            void setLoop(oniSoundID id, bool loop) override;

            void setVolume(oniSoundID id, common::real32 volume) override;

            common::real32 getVolume(oniSoundID id) override;

            bool isPlaying(oniSoundID id) override;

            void seek(oniSoundID id, oni::common::real64 position) override;

            void setPitch(oniSoundID id, common::real32 pitch) override;

        private:
            std::unique_ptr<FMOD::System, FMODDeleter> mSystem;
            std::vector<std::unique_ptr<FMOD::Sound, FMODDeleter>> mSounds;
            std::vector<std::unique_ptr<FMOD::Channel, FMODDeleter>> mChannels;
        };
    }
}