#include <oni-core/audio/audio-manager-fmod.h>

#include <cassert>

#include <fmod.hpp>

#include <oni-core/common/consts.h>
#include <oni-core/common/defines.h>

#define ERRCHECK(_result) assert((_result) == FMOD_OK)

namespace oni {
    namespace audio {
        AudioManagerFMOD::AudioManagerFMOD() : AudioManager(), mSystem{}, mSounds{}, mLoopingSoundChannel{} {
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

        SoundID AudioManagerFMOD::loadSound(const std::string &name) {
            FMOD::Sound *sound{nullptr};
            auto result = mSystem->createSound(name.c_str(), FMOD_DEFAULT, nullptr, &sound);
            mSounds.emplace_back(std::unique_ptr<FMOD::Sound, FMODDeleter>(sound, FMODDeleter()));
            ERRCHECK(result);

            // TODO: This is mixing the semantics and overlaping SoundID with loadLoopingSound(). Design is a bit messy
            // I have to refactor it so that for users there is just SoundID, and they could play it once, or loop it.
            // I can create a channel for one shot sounds as well, but that just sounds stupid to create channel
            // and destroy it at the end. I might need it for sounds that are played once but while playing I need
            // to adjust them, e.g., change pitch.
            return mSounds.size() - 1;
        }

        SoundID AudioManagerFMOD::loadLoopingSound(const std::string &name) {
            auto soundID = loadSound(name);

            FMOD::Channel *channel{nullptr};
            auto result = mSystem->playSound(mSounds[soundID].get(), nullptr, true, &channel);
            ERRCHECK(result);

            mLoopingSoundChannel.emplace_back(std::unique_ptr<FMOD::Channel, FMODDeleter>(channel, FMODDeleter()));

            return mLoopingSoundChannel.size() - 1;
        }

        void AudioManagerFMOD::playLoopingSound(SoundID id) {
            assert(id >= 0 && id < mLoopingSoundChannel.size());

            auto result = mLoopingSoundChannel[id]->setMode(FMOD_LOOP_NORMAL);
            ERRCHECK(result);

            result = mLoopingSoundChannel[id]->setPaused(false);
            ERRCHECK(result);
        }

        void AudioManagerFMOD::playSoundOnce(SoundID id) {
            assert(id >= 0 && id < mSounds.size());

            auto result = mSystem->playSound(mSounds[id].get(), nullptr, false, nullptr);
            ERRCHECK(result);
        }

        double AudioManagerFMOD::pauseSound(SoundID id) {
            assert(id >= 0 && id < mLoopingSoundChannel.size());

            auto result = mLoopingSoundChannel[id]->setPaused(true);
            ERRCHECK(result);

            common::uint32 pos;
            result = mLoopingSoundChannel[id]->getPosition(&pos, FMOD_TIMEUNIT_MS);
            ERRCHECK(result);

            // TODO: This is just a work around to keep the interface consistent with double.
            return (static_cast<double>(pos) + common::EP);
        }

        void AudioManagerFMOD::stopSound(SoundID id) {
            UNUSED(id);
        }

        void AudioManagerFMOD::setLoop(SoundID id, bool loop) {
            UNUSED(id);
            UNUSED(loop);
        }

        void AudioManagerFMOD::setVolume(SoundID id, common::real32 volume) {
            auto result = mLoopingSoundChannel[id]->setVolume(volume);
            ERRCHECK(result);
        }

        bool AudioManagerFMOD::isPlaying(SoundID id) {
            bool isPaused{false};
            auto result = mLoopingSoundChannel[id]->getPaused(&isPaused);
            ERRCHECK(result);
            return !isPaused;
        }

        void AudioManagerFMOD::seek(SoundID id, double position) {
            auto result = mLoopingSoundChannel[id]->setPosition(static_cast<common::uint32>(position + common::EP),
                                                     FMOD_TIMEUNIT_MS);
            ERRCHECK(result);
        }

        void AudioManagerFMOD::setPitch(SoundID id, common::real32 pitch) {
            if (pitch > 256) {
                pitch = 256;
            }
            auto result = mLoopingSoundChannel[id]->setPitch(pitch);
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