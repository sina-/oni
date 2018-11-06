#pragma once

#include <oni-core/common/typedefs.h>
#include <oni-core/math/vec2.h>

namespace oni {
    namespace components {

        // TODO: Not sure if this is the right place to define this
        enum class WallTilePosition : common::int8 {
            TOP = 1,
            RIGHT = 2,
            BOTTOM = 3,
            LEFT = 4,
        };

        struct Checkpoint {
            math::vec2 position;
            common::real32 orientation;
        };
    }
}
