#pragma once

#include <GL/glew.h>
#include <string>
#include <entities/world.h>
#include <ftgl/texture-atlas.h>
#include "font-manager.h"

namespace oni {
    namespace graphics {
        class LoadTexture {
            LoadTexture() = default;

            ~LoadTexture() = default;

        public:
            static components::Texture load(const std::string &path);

            static components::Texture load(graphics::FontManager &fontManager);

            static void bind(GLuint textureID);

            static void unbind();
        };
    }
}