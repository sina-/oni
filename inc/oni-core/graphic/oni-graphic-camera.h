#pragma once

#include <unordered_map>

#include <oni-core/common/oni-common-typedef.h>


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

    enum class ZLayerDef : oni::u8 {
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
    using ZLayer = std::unordered_map<ZLayerDef, r32>;
}
