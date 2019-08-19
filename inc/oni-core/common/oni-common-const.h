#pragma once

#include <oni-core/common/oni-common-typedef.h>


namespace oni {
    // TODO: This is really not a good way to define epsilon, this value is usually very context sensitive, meaning
    // different epsilons are desirable for different scales of calculation in the engine and there is no one
    // catch all value. My approximate math function need to accept epsilon as argument.
    static constexpr r32 EP32 = 0.0001f;
    static constexpr r64 EP64 = std::numeric_limits<r64>::epsilon();
    static constexpr i32 i32Max = std::numeric_limits<i32>::max();
    static constexpr u32 u32Max = std::numeric_limits<u32>::max();
    static constexpr i64 i64Max = std::numeric_limits<i64>::max();
    static constexpr u64 u64Max = std::numeric_limits<u64>::max();
    static constexpr u32 maxNumTextureSamplers{32};
    static constexpr r32 PI = 3.14159265358979323846f;
    static constexpr r32 HALF_PI = 3.14159265358979323846f / 2.f;
    static constexpr r32 TWO_PI = PI * 2;
    static constexpr r32 FULL_CIRCLE_IN_RAD = TWO_PI;
    static constexpr u32 R_MASK = 0xff000000;
    static constexpr u32 G_MASK = 0x00ff0000;
    static constexpr u32 B_MASK = 0x0000ff00;
    static constexpr u32 A_MASK = 0x000000ff;
}
