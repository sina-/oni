#pragma once

#include <oni-core/component/oni-component-visual.h>

namespace oni {
    namespace graphic {
        struct Brush {
            component::BrushType type{component::BrushType::UNKNOWN};
            union {
                component::PixelRGBA color{};
                component::TextureTag tag;
            };
        };
    }
}
