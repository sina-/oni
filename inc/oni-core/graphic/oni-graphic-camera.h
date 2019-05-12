#pragma once

#include <unordered_map>

#include <oni-core/common/oni-common-typedef.h>

namespace oni {
    namespace graphic {
        struct ScreenBounds {
            common::real32 xMin;
            common::real32 xMax;
            common::real32 yMin;
            common::real32 yMax;
        };

        struct Camera {
            common::real32 x;
            common::real32 y;
            common::real32 z;
        };

        enum class ZLayerDef : common::uint8 {
            LAYER_0 = 0,
            LAYER_1 = 1,
            LAYER_2 = 2,
            LAYER_3 = 3,
            LAYER_4 = 4,
            LAYER_5 = 5,
            LAYER_6 = 6,
            LAYER_7 = 7,
            LAYER_8 = 8,
            LAYER_9 = 9,
        };
        using ZLayer = std::unordered_map<ZLayerDef, common::real32>;
    }
}
