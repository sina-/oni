#pragma once

#include <oni-core/component/oni-component-visual.h>

namespace oni {
    namespace graphic {
        struct Brush {
            component::BrushType type{component::BrushType::UNKNOWN};
            union {
                component::Color color{};
                component::TextureTag tag;
                common::oniGLuint textureID;
            };
        };
    }
}
