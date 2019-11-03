#pragma once

#include <oni-core/util/oni-util-enum.h>

namespace oni {
    enum class SoundPlaybackState : oni::u8 {
        PLAY,
        STOP,
        FADE_OUT,
    };

    enum class ChannelGroup : oni::u8 {
        UNKNOWN,

        MUSIC,
        EFFECT,

        LAST
    };

    using SoundName = HashedString;

    struct Sound {
        SoundName name = {};
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
