#pragma once

#include <oni-core/component/oni-component-visual.h>

namespace oni {
    struct Brush {
        Brush() {}

        ~Brush() {}

        BrushType type{BrushType::COLOR};
        union {
            // TODO: Think about this, life-times! I probably need shared pointer, but I don't want to pay the
            // price for the pointer house-keeping. Textures all come from TextureManager and that manager
            // guarantees the life-time of the textures.
            const Texture *texture{};
            const Color *color;
            EntityAssetsPack tag;
        };
        const Quad *shape_Quad{};
        const mat4 *model{}; // NOTE: Optional model transformation matrix if the quad is untransformed
    };
}
