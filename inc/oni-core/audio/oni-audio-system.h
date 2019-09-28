#pragma once

#include <oni-core/system/oni-system.h>
#include <oni-core/audio/oni-audio-fwd.h>

namespace oni {
    class System_CarEnginePitch : public SystemTemplate<
            Car,
            Sound,
            SoundPitch> {
    public:
        explicit System_CarEnginePitch(EntityManager &);

    protected:
        void
        update(EntityTickContext &context,
               Car &,
               Sound &,
               SoundPitch &) override;

        void
        postUpdate(EntityManager &mng,
                   duration32 dt) override;
    };

    class System_PlayPauseSound : public SystemTemplate<
            Sound,
            WorldP3D> {
    public:
        System_PlayPauseSound(EntityManager &,
                              AudioManager &);

    protected:
        void
        update(EntityTickContext &,
               Sound &,
               WorldP3D &) override;

        void
        postUpdate(EntityManager &mng,
                   duration32 dt) override;

    private:
        AudioManager &mAudioMng;
    };
}