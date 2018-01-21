#pragma once

#include "renderable2d.h"
#include "shader.h"

namespace oni {
    namespace graphics {

        class Sprite : public Renderable2D {
        public:
            Sprite(const math::vec2 &size, const math::vec3 &pos, const math::vec4 &color, Shader &shader)
                    : Renderable2D(size, pos, color) {}

        };

    }
}