#pragma once

#include <oni-core/common/oni-common-typedef.h>

namespace oni {
    namespace common {
        static constexpr r32 EP = std::numeric_limits<r32>::epsilon();
        static constexpr u32 maxNumTextureSamplers{32};
        static constexpr r32 PI = 3.14159265358979323846f;
        static constexpr r32 PI_TIMES_TWO = PI * 2;
        static constexpr r32 FULL_CIRCLE_IN_RAD = PI_TIMES_TWO;
    }
}
