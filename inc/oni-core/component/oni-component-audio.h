#pragma once

namespace oni {
    enum class SoundPlaybackState : oni::u8 {
        PLAY,
        STOP,
        FADE_OUT,
    };

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
    };
}
