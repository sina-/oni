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
        using common::OniSoundID;

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

            OniSoundID loadSound(const std::string &name) override;

            void playSound(OniSoundID id) override;

            oni::common::real64 pauseSound(OniSoundID id) override;

            void stopSound(OniSoundID id) override;

            void setLoop(OniSoundID id, bool loop) override;

            void setVolume(OniSoundID id, common::real32 volume) override;

            common::real32 getVolume(OniSoundID id) override;

            bool isPlaying(OniSoundID id) override;

            void seek(OniSoundID id, oni::common::real64 position) override;

            void setPitch(OniSoundID id, common::real32 pitch) override;

        private:
            std::unique_ptr<FMOD::System, FMODDeleter> mSystem;
            std::vector<std::unique_ptr<FMOD::Sound, FMODDeleter>> mSounds;
            std::vector<std::unique_ptr<FMOD::Channel, FMODDeleter>> mChannels;
        };
    }
}