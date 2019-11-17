#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/util/oni-util-enum.h>


namespace oni {
    struct ScreenBounds {
        r32 xMin{};
        r32 xMax{};
        r32 yMin{};
        r32 yMax{};
    };

    struct Camera {
        r32 x{};
        r32 y{};
        r32 z{};
    };

    ONI_ENUM_DEF(ZLayer,
                 { 0, "LAYER_0" },
                 { 1, "LAYER_1" },
                 { 2, "LAYER_2" },
                 { 3, "LAYER_3" },
                 { 4, "LAYER_4" },
                 { 5, "LAYER_5" },
                 { 6, "LAYER_6" },
                 { 7, "LAYER_7" },
                 { 8, "LAYER_8" },
                 { 9, "LAYER_9" },
    )
}
