#include <fmod.hpp>

#include <oni-core/audio/audio-manager-fmod.h>
#include <oni-core/utils/oni-assert.h>
#include <oni-core/common/consts.h>

#define ERRCHECK(_result) ONI_DEBUG_ASSERT((_result) == FMOD_OK)

namespace oni {
    namespace audio {

        AudioManagerFMOD::AudioManagerFMOD() {

            FMOD::System *system = nullptr;

            auto result = FMOD::System_Create(&system);
            ERRCHECK(result);
            mSystem = std::unique_ptr<FMOD::System, FMODDeleter>(system, FMODDeleter());

            unsigned int version;
            result = system->getVersion(&version);
            ERRCHECK(result);
            ONI_DEBUG_ASSERT(version >= FMOD_VERSION)

            result = system->init(32, FMOD_INIT_NORMAL, nullptr);
            ERRCHECK(result);

            result = system->update();
            ERRCHECK(result);
        }

        AudioManagerFMOD::~AudioManagerFMOD() = default;

        void AudioManagerFMOD::tick() {
            auto result = mSystem->update();
            ERRCHECK(result);
        }

        oniSoundID AudioManagerFMOD::loadSound(const std::string &name) {
            // TODO: This will break for one-off sound effects, currently all the functions in this
            // class work if the audio is to be looped over.

            FMOD::Sound *sound;
            auto result = mSystem->createSound(name.c_str(), FMOD_DEFAULT, nullptr, &sound);
            mSounds.emplace_back(std::unique_ptr<FMOD::Sound, FMODDeleter>(sound, FMODDeleter()));
            ERRCHECK(result);

            result = mSounds.back()->setMode(FMOD_LOOP_NORMAL);
            ERRCHECK(result);

            FMOD::Channel *channel;
            result = mSystem->playSound(sound, nullptr, true, &channel);
            ERRCHECK(result);

            mChannels.emplace_back(std::unique_ptr<FMOD::Channel>(channel));

            return mChannels.size() - 1;
        }

        void AudioManagerFMOD::playSound(oniSoundID id) {
            auto result = mChannels[id]->setPaused(false);
            ERRCHECK(result);
        }

        double AudioManagerFMOD::pauseSound(oniSoundID id) {
            auto result = mChannels[id]->setPaused(true);
            ERRCHECK(result);

            unsigned int pos;
            result = mChannels[id]->getPosition(&pos, FMOD_TIMEUNIT_MS);
            ERRCHECK(result);

            // TODO: This is just a work around to keep the interface consistent with double.
            return ((double) pos) + common::ep;
        }

        void AudioManagerFMOD::stopSound(oniSoundID id) {

        }

        void AudioManagerFMOD::setLoop(oniSoundID id, bool loop) {

        }

        void AudioManagerFMOD::setVolume(oniSoundID id, float volume) {

        }

        float AudioManagerFMOD::getVolume(oniSoundID id) {
            return 0;
        }

        bool AudioManagerFMOD::isPlaying(oniSoundID id) {
            bool isPaused = false;
            auto result = mChannels[id]->getPaused(&isPaused);
            ERRCHECK(result);
            return !isPaused;
        }

        void AudioManagerFMOD::seek(oniSoundID id, double position) {
            auto result = mChannels[id]->setPosition(static_cast<unsigned int>(position + common::ep), FMOD_TIMEUNIT_MS);
            ERRCHECK(result);
        }

        void AudioManagerFMOD::setPitch(oniSoundID id, float pitch) {
            auto result = mChannels[id]->setPitch(pitch);
            ERRCHECK(result);
        }

        void FMODDeleter::operator()(FMOD::Sound *s) const {
            s->release();
        }

        void FMODDeleter::operator()(FMOD::System *sys) const {
            sys->close();
            sys->release();
        }
    }
}