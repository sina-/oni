#pragma once

#include <string>
#include <oni-core/math/vec2.h>

namespace oni {
    namespace component {
        enum class SoundPlaybackState : common::uint8 {
            PLAY,
            STOP,
            FADE_OUT,
        };

        enum class SoundTag : common::uint32 {
            ROCKET,
            ENGINE_IDLE,
        };

        using SoundID = std::string;
        using SoundPos = math::vec2;
    }
}