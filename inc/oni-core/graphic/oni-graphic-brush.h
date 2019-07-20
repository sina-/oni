#pragma once

#include <oni-core/component/oni-component-visual.h>

namespace oni {
    namespace graphic {
        struct Brush {
            Brush() {}

            ~Brush() {}

            component::BrushType type{component::BrushType::UNKNOWN};
            union {
                // TODO: Think about this, life-times! I probably need shared pointer, but I don't want to pay the
                // price for the pointer house-keeping. Textures all come from TextureManager and that manager
                // guarantees the life-time of the textures.
                component::Texture *texture{};
                component::Color *color;
                component::TextureTag tag;
            };
            component::Quad *shape_Quad;
        };
    }
}
