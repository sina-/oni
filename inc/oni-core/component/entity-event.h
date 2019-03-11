#pragma once

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace component {
        enum class EventType : common::uint8 {
            COLLISION,
            SPAWN_PARTICLE,
            SOUND_EFFECT,
        };
    }
}