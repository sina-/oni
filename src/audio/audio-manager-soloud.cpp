#include <algorithm>
#include <memory>

#include <oni-audio/audio-manager-soloud.h>

namespace oni {
    namespace audio {

        AudioManagerSoloud::AudioManagerSoloud() {
            using namespace SoLoud;
            mSoloudManager = SoLoud::Soloud();
            mSoloudManager.init(Soloud::FLAGS::CLIP_ROUNDOFF, Soloud::BACKENDS::SDL2, Soloud::AUTO, Soloud::AUTO);
        }

        AudioManagerSoloud::~AudioManagerSoloud() {
            mSoloudManager.deinit();
        }

        oniSoundID AudioManagerSoloud::loadSound(const std::string &name) {
            // Avoid double loading
            auto it = std::find(mAudioNames.begin(), mAudioNames.end(), name);

            if (it == mAudioNames.end()) {
                mAudioNames.emplace_back(name);
                auto wav = std::make_unique<SoLoud::Wav>();
                auto result = wav->load(name.c_str());
                ONI_DEBUG_ASSERT(result == SoLoud::SOLOUD_ERRORS::SO_NO_ERROR);

                // Get a handle to a paused audio. Use the handle to play the audio.
                auto handle = mSoloudManager.play(*wav, 1, 0, true);
                mHandles.push_back(handle);
                mAudioSources.emplace_back(std::move(wav));
                mSoloudManager.setRelativePlaySpeed(handle, 0.3f);

                // This will map internal id to range of 1...max and hide the internal id from the users.
                // It is useful when iterating over the sounds and accessing them in a memory aligned fashion
                // but until the problem with stack allocation of mAudioSources is fixed, this is a useless
                // abstraction.
                return mHandles.size() - 1;
            } else {
                return it - mAudioNames.begin();
            }
        }

        void AudioManagerSoloud::playSound(oniSoundID id) {
            if (mSoloudManager.getPause(mHandles[id])) {
                mSoloudManager.setPause(mHandles[id], false);
            } else {
                // NOTE: if play is called on paused source, none of the attribute settings will work.
                mSoloudManager.play(*mAudioSources[id]);
            }
        }

        double AudioManagerSoloud::pauseSound(oniSoundID id) {
            mSoloudManager.setPause(mHandles[id], true);
            return mSoloudManager.getStreamTime(mHandles[id]);
            return 0;
        }

        void AudioManagerSoloud::setLoop(oniSoundID id, bool loop) {
            mSoloudManager.setLooping(mHandles[id], loop);
        }

        void AudioManagerSoloud::stopSound(oniSoundID id) {
            mSoloudManager.stop(mHandles[id]);
        }

        void AudioManagerSoloud::setVolume(oniSoundID id, common::real32 volume) {
            mSoloudManager.setVolume(mHandles[id], volume);
        }

        common::real32 AudioManagerSoloud::getVolume(oniSoundID id) {
            mSoloudManager.getVolume(mHandles[id]);
            return 0;
        }

        bool AudioManagerSoloud::isPlaying(oniSoundID id) {
            return !mSoloudManager.getPause(mHandles[id]);
            return false;
        }

        void AudioManagerSoloud::seek(oniSoundID id, double offset) {
            mSoloudManager.seek(mHandles[id], offset);
        }

    }
}
