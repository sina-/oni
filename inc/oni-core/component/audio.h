#pragma once

#include <string>
#include <oni-core/math/vec2.h>

namespace oni {
    namespace component {
        enum class SoundPlaybackState : common::uint8 {
            REQUIRES_PLAYBACK,
            PLAYING,
            STOPPED,
        };

        using SoundID = std::string;
        using SoundPos = math::vec2;
    }
}