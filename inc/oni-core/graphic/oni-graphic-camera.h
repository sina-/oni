#pragma once

#include <unordered_map>

#include <oni-core/common/oni-common-typedef.h>

namespace oni {
    namespace graphic {
        struct ScreenBounds {
            common::r32 xMin;
            common::r32 xMax;
            common::r32 yMin;
            common::r32 yMax;
        };

        struct Camera {
            common::r32 x;
            common::r32 y;
            common::r32 z;
        };

        enum class ZLayerDef : common::u8 {
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
        using ZLayer = std::unordered_map<ZLayerDef, common::r32>;
    }
}
