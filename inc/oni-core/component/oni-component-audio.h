#pragma once

#include <string>
#include <oni-core/math/oni-math-vec2.h>

namespace oni {
    namespace component {
        enum class SoundPlaybackState : common::u8 {
            PLAY,
            STOP,
            FADE_OUT,
        };

        enum class SoundTag : common::u32 {
            UNKNOWN,

            ROCKET_LAUNCH,
            ROCKET_BURN,
            ENGINE_IDLE,

            COLLISION_ROCKET_UNKNOWN,

            LAST
        };

        enum class ChannelGroup : common::u8 {
            UNKNOWN,

            MUSIC,
            EFFECT,

            LAST
        };

        struct Sound {
            SoundTag tag = SoundTag::UNKNOWN;
            ChannelGroup group = ChannelGroup::UNKNOWN;
            common::r32 pitch = 1.f;
        };
    }
}