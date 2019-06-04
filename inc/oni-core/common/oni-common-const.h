#pragma once

#include <oni-core/common/oni-common-typedef.h>

namespace oni {
    namespace common {
        // TODO: This is really not a good way to define epsilon, this value is usually very context sensitive, meaning
        // different epsilons are desirable for different scales of calculation in the engine and there is no one
        // catch all value. My approximate math function need to accept epsilon as argument.
        static constexpr r32 EP32 = 0.0001f; //std::numeric_limits<r32>::epsilon();
        static constexpr r64 EP64 = std::numeric_limits<r64>::epsilon();
        static constexpr i32 i32Max = std::numeric_limits<i32>::max();
        static constexpr u32 u32Max = std::numeric_limits<u32>::max();
        static constexpr i64 i64Max = std::numeric_limits<i64>::max();
        static constexpr u64 u64Max = std::numeric_limits<u64>::max();
        static constexpr u32 maxNumTextureSamplers{32};
        static constexpr r32 PI = 3.14159265358979323846f;
        static constexpr r32 TWO_PI = PI * 2;
        static constexpr r32 FULL_CIRCLE_IN_RAD = TWO_PI;
    }
}
