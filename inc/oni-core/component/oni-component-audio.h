#pragma once

#include <oni-core/util/oni-util-enum.h>
#include <oni-core/asset/oni-asset-structure.h>

namespace oni {
    enum class SoundPlaybackState : oni::u8 {
        PLAY,
        STOP,
        FADE_OUT,
    };

    // TODO: Use the new ENUM
    enum class ChannelGroup : oni::u8 {
        UNKNOWN,

        MUSIC,
        EFFECT,

        LAST
    };

    struct Sound {
        SoundName name = {};
        ChannelGroup group = ChannelGroup::UNKNOWN;
    };

    struct SoundPitch {
        r32 value = 1.f;
    };
}
