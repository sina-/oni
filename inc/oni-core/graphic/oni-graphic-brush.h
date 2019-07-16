#pragma once

#include <oni-core/component/oni-component-visual.h>

namespace oni {
    namespace graphic {
        struct Brush {
            Brush() {}
            ~Brush() {}

            component::BrushType type{component::BrushType::UNKNOWN};
            union {
                component::Color color;
                component::TextureTag tag;
                component::Texture texture{};
            };
        };
    }
}
