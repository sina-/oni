#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-physics.h>

namespace oni {
    namespace graphic {
        struct BrushTrail {
            common::r32 mass{1};
            component::Heading2D heading{};
            component::WorldP2D last{};
            component::WorldP2D lastDelta{};
            component::WorldP2D current{};
            component::Velocity2D velocity2d{};
            component::Velocity velocity{};
            component::Acceleration acceleration{};
            component::Acceleration2D acceleration2d{};
            std::vector<component::WorldP2D> vertices;
        };

        void
        testDraw(BrushTrail *f,
                 common::r32 x,
                 common::r32 y);

        void
        filtersetpos(BrushTrail *f,
                     common::r32 x,
                     common::r32 y);
    }
}

