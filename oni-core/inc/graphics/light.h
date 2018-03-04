#pragma once

#include <graphics/window.h>
#include <graphics/layer.h>

namespace oni {
    namespace graphics {
        class Light {
        public:
            static void update(Layer &layer, double mouseX, double mouseY, int windowWidth, int windowHeight);
        };
    }
}