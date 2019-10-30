#pragma once

#include <oni-core/util/oni-util-enum.h>

namespace oni {
    enum class SoundPlaybackState : oni::u8 {
        PLAY,
        STOP,
        FADE_OUT,
    };

    ONI_ENUM_DEF(SoundTag,
                 { 0, "Unknown" },
                 { 1, "Rocket_Launch" },
                 { 2, "Rocket_Burn" },
                 { 3, "Engine_Idle" },
                 { 4, "Collision_Rocket_Unknown" }
    );

    enum class Sound_Tag : oni::u32 {
        UNKNOWN,

        ROCKET_LAUNCH,
        ROCKET_BURN,
        ENGINE_IDLE,

        COLLISION_ROCKET_UNKNOWN,

        LAST
    };

    enum class ChannelGroup : oni::u8 {
        UNKNOWN,

        MUSIC,
        EFFECT,

        LAST
    };

    struct Sound {
        Sound_Tag tag = Sound_Tag::UNKNOWN;
        ChannelGroup group = ChannelGroup::UNKNOWN;
    };

    struct SoundPitch {
        r32 value = 1.f;

        template<class Archive>
        void
        serialize(Archive &archive) {
            archive("value", value);
        }
    };
}
