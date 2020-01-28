#pragma once

#include <oni-core/util/oni-util-enum.h>
#include <oni-core/asset/oni-asset-structure.h>

namespace oni {
    enum class SoundPlaybackState : oni::u8 {
        PLAY,
        STOP,
        FADE_OUT,
    };

    ONI_ENUM_DEF(ChannelGroup, {0, "unknown"}, {1, "music"}, {2, "effect"})

    struct Sound {
        SoundName name = {};
        ChannelGroup group = ChannelGroup::GET("unknown");
    };

    struct SoundPitch {
        r32 value = 1.f;
    };
}
