#include <oni-core/audio/audio-manager-fmod.h>
#include <oni-core/utils/oni-assert.h>

namespace oni {
    namespace audio {

        AudioManagerFmod::AudioManagerFmod() {

        }

        AudioManagerFmod::~AudioManagerFmod() {

        }

        oniSoundID AudioManagerFmod::loadSound(const std::string &name) {
            return 0;
        }

        void AudioManagerFmod::playSound(oniSoundID id) {

        }

        double AudioManagerFmod::pauseSound(oniSoundID id) {
            return 0;
        }

        void AudioManagerFmod::stopSound(oniSoundID id) {

        }

        void AudioManagerFmod::setLoop(oniSoundID id, bool loop) {

        }

        void AudioManagerFmod::setVolume(oniSoundID id, float volume) {

        }

        float AudioManagerFmod::getVolume(oniSoundID id) {
            return 0;
        }

        bool AudioManagerFmod::isPlaying(oniSoundID id) {
            return false;
        }

        void AudioManagerFmod::seek(oniSoundID id, double position) {

        }
    }
}