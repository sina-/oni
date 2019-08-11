#pragma once

#include <vector>

#include <oni-core/common/oni-common-typedef.h>

namespace oni {
    namespace component {
        enum class ComponentType : common::u16 {
            UNKNOWN,

            SMOKE_EMITTER_CD,
            SOUND_PITCH,
            BRUSH_TRAIL,

            LAST
        };
    }
}
