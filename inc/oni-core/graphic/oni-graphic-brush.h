#pragma once

#include <oni-core/component/oni-component-visual.h>

namespace oni {
    struct Brush {
        Brush() {}

        ~Brush() {}

        BrushType type{BrushType::COLOR};
        Material_Definition material{};
        const Quad *shape_Quad{};
        const mat4 *model{}; // NOTE: Optional model transformation matrix if the quad is untransformed
    };
}
