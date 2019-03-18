#pragma once

#include <vector>
#include <memory>
#include <map>

#include <oni-core/audio/audio-manager.h>
#include <oni-core/common/typedefs.h>

namespace FMOD {
    class Sound;

    class System;

    class Channel;
}

namespace oni {
    namespace audio {
        class AudioManagerFMOD : public AudioManager {
        public:
            AudioManagerFMOD();

            void tick(entities::EntityFactory&) override;

            void loadSound(const component::SoundID &) override;

            void attachControls(const component::SoundID &id) override;

            void play(const component::SoundID &id) override;

            void setPitch(const component::SoundID &, common::real32 pitch) override;

            void setLoop(const component::SoundID &, bool loop) override;

            common::real64 pauseSound(const component::SoundID &) override;

            void fadeOut(const component::SoundID &id) override;

            void setVolume(const component::SoundID &, common::real32 volume) override;

            bool isPlaying(const component::SoundID&) override;

            void seek(const component::SoundID &, common::real64 position) override;

        private:
            class FMODDeleter {
            public:
                void operator()(FMOD::Sound *s) const;

                void operator()(FMOD::System *sys) const;

                void operator()(FMOD::Channel *channel) const;
            };

        private:
            std::unique_ptr<FMOD::System, FMODDeleter> mSystem;
            std::unordered_map<component::SoundID, std::unique_ptr<FMOD::Sound, FMODDeleter>> mSounds;
            std::map<component::SoundID, std::unique_ptr<FMOD::Channel, FMODDeleter>> mChannels;
        };
    }
}