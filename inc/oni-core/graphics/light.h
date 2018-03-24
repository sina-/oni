#pragma once

#include <oni-core/graphics/window.h>
#include <oni-core/graphics/layer.h>

namespace oni {
    namespace graphics {
        class Light {
        public:
            static void update(Layer &layer, double mouseX, double mouseY, int windowWidth, int windowHeight);
        };
    }
}