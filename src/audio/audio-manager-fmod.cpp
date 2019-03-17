#include <oni-core/audio/audio-manager-fmod.h>

#include <cassert>

#include <fmod.hpp>

#include <oni-core/common/consts.h>
#include <oni-core/common/defines.h>

#define ERRCHECK(_result) assert((_result) == FMOD_OK)
#define VALID(MAP, ID) assert(MAP.find(id) != MAP.end())

namespace oni {
    namespace audio {
        AudioManagerFMOD::AudioManagerFMOD() : AudioManager(), mSystem{}, mSounds{}, mChannels{} {
            FMOD::System *system{nullptr};

            auto result = FMOD::System_Create(&system);
            ERRCHECK(result);
            mSystem = std::unique_ptr<FMOD::System, FMODDeleter>(system, FMODDeleter());

            common::uint32 version;
            result = system->getVersion(&version);
            ERRCHECK(result);
            assert(version >= FMOD_VERSION);

            result = system->init(32, FMOD_INIT_NORMAL, nullptr);
            ERRCHECK(result);

            result = system->update();
            ERRCHECK(result);
        }

        void AudioManagerFMOD::tick() {
            auto result = mSystem->update();
            ERRCHECK(result);
        }

        void AudioManagerFMOD::loadSound(const component::SoundID &id) {
            FMOD::Sound *sound{};
            auto result = mSystem->createSound(id.c_str(), FMOD_DEFAULT, nullptr, &sound);
            ERRCHECK(result);
            assert(sound);

            mSounds[id] = std::unique_ptr<FMOD::Sound, FMODDeleter>(sound, FMODDeleter());
        }

        void AudioManagerFMOD::attachControls(const component::SoundID &id) {
            VALID(mSounds, id);

            FMOD::Channel *channel{nullptr};
            auto result = mSystem->playSound(mSounds[id].get(), nullptr, true, &channel);
            ERRCHECK(result);

            mChannels[id] = std::unique_ptr<FMOD::Channel, FMODDeleter>(channel, FMODDeleter());
        }

        void AudioManagerFMOD::play(const component::SoundID &id) {
            if (mChannels.find(id) != mChannels.end()) {
                // NOTE: For non-looping sounds, after one play through the channel is not valid anymore.
                auto result = mChannels[id]->setPaused(false);
                ERRCHECK(result);
            } else {
                VALID(mSounds, id);
                auto result = mSystem->playSound(mSounds[id].get(), nullptr, false, nullptr);
                ERRCHECK(result);
            }
        }

        void AudioManagerFMOD::setLoop(const component::SoundID &id, bool loop) {
            VALID(mChannels, id);

            if (loop) {
                mChannels[id]->setMode(FMOD_LOOP_NORMAL);
            } else {
                mChannels[id]->setMode(FMOD_LOOP_OFF);
            }
        }

        void AudioManagerFMOD::setPitch(const component::SoundID &id, common::real32 pitch) {
            if (pitch > 256) {
                pitch = 256;
            }
            VALID(mChannels, id);
            auto result = mChannels[id]->setPitch(pitch);
            ERRCHECK(result);
        }

        common::real64 AudioManagerFMOD::pauseSound(const component::SoundID &id) {
            VALID(mChannels, id);

            auto result = mChannels[id]->setPaused(true);
            ERRCHECK(result);

            common::uint32 pos;
            result = mChannels[id]->getPosition(&pos, FMOD_TIMEUNIT_MS);
            ERRCHECK(result);

            return (static_cast<common::real64>(pos) + common::EP);
        }

        void AudioManagerFMOD::setVolume(const component::SoundID &id, common::real32 volume) {
            VALID(mChannels, id);
            auto result = mChannels[id]->setVolume(volume);
            ERRCHECK(result);
        }

        bool AudioManagerFMOD::isPlaying(const component::SoundID &id) {
            VALID(mChannels, id);
            bool isPaused{false};
            auto result = mChannels[id]->getPaused(&isPaused);
            ERRCHECK(result);
            return !isPaused;
        }

        void AudioManagerFMOD::seek(const component::SoundID &id, common::real64 position) {
            VALID(mChannels, id);
            auto result = mChannels[id]->setPosition(static_cast<common::uint32>(position + common::EP),
                                                     FMOD_TIMEUNIT_MS);
            ERRCHECK(result);
        }

        void AudioManagerFMOD::FMODDeleter::operator()(FMOD::Sound *s) const {
            s->release();
        }

        void AudioManagerFMOD::FMODDeleter::operator()(FMOD::System *sys) const {
            sys->close();
            sys->release();
        }

        void AudioManagerFMOD::FMODDeleter::operator()(FMOD::Channel *channel) const {
            UNUSED(channel);
        }
    }
}