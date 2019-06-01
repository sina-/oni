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
            ROCKET,
            ENGINE_IDLE,
        };

        struct SoundID {
            std::string value;
        };

        enum class ChannelGroup : common::u8 {
            MUSIC,
            EFFECT,

            SIZE
        };
    }
}