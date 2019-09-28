#include <oni-core/audio/oni-audio-system.h>

#include <oni-core/audio/oni-audio-manager.h>

namespace oni {
    System_PlayPauseSound::System_PlayPauseSound(EntityManager &em,
                                                 AudioManager &am) : SystemTemplate(em), mAudioMng(am) {}

    void
    System_PlayPauseSound::update(EntityTickContext &etc,
                                  Sound &sound,
                                  WorldP3D &pos) {
        auto &entityChannel = mAudioMng.getOrCreateLooping3DChannel(sound, etc.id);
        auto distance = (pos.value - mAudioMng.mPlayerPos.value).len();
        if (distance < mAudioMng.mMaxAudibleDistance) {
            if (etc.mng.has<SoundPitch>(etc.id)) {
                auto &pitch = etc.mng.get<SoundPitch>(etc.id);
                mAudioMng.setPitch(*entityChannel.channel, pitch.value);
            }

            auto v = vec3{1.f, 0.f, 0.f}; // TODO: Does it matter if this is accurate?
            mAudioMng.set3DPos(*entityChannel.channel, pos.value - mAudioMng.mPlayerPos.value, v);
            mAudioMng.unPause(*entityChannel.channel);
        } else {
            mAudioMng.pause(*entityChannel.channel);
        }
    }

    void
    System_PlayPauseSound::postUpdate(EntityManager &mng,
                                      duration32 dt) {}
}