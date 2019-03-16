#pragma once

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
        using common::SoundID;

        class AudioManagerFMOD : public AudioManager {
        public:
            AudioManagerFMOD();

            void tick() override;

            SoundID loadSound(const std::string &name) override;

            SoundID loadLoopingSound(const std::string &name) override;

            void playLoopingSound(SoundID id) override;

            void playSoundOnce(SoundID id) override;

            oni::common::real64 pauseSound(SoundID id) override;

            void stopSound(SoundID id) override;

            void setLoop(SoundID id, bool loop) override;

            void setVolume(SoundID id, common::real32 volume) override;

            common::real32 getVolume(SoundID id) override;

            bool isPlaying(SoundID id) override;

            void seek(SoundID id, oni::common::real64 position) override;

            void setPitch(SoundID id, common::real32 pitch) override;

        private:
            class FMODDeleter {
            public:
                void operator()(FMOD::Sound *s) const;

                void operator()(FMOD::System *sys) const;

                void operator()(FMOD::Channel *channel) const;
            };

        private:
            std::unique_ptr <FMOD::System, FMODDeleter> mSystem;
            std::vector <std::unique_ptr<FMOD::Sound, FMODDeleter>> mSounds;
            std::vector <std::unique_ptr<FMOD::Channel, FMODDeleter>> mLoopingSoundChannel;
        };
    }
}