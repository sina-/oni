#include <oni-core/audio/oni-audio-system.h>

namespace oni {
    System_CarEnginePitch::System_CarEnginePitch(EntityManager &em) : SystemTemplate(em) {}

    void
    System_CarEnginePitch::update(EntityTickContext &etc,
                                   Car &car,
                                   Sound &sound,
                                   SoundPitch &pitch) {
        pitch.value = static_cast< r32>(car.rpm) / 2000;
    }

    void
    System_CarEnginePitch::postUpdate(EntityManager &mng,
                                       duration32 dt) {}
}