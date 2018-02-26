#pragma once

#include <GL/glew.h>
#include <string>
#include <entities/world.h>

namespace oni {
    namespace graphics {
        class LoadTexture {
            LoadTexture() = default;

            ~LoadTexture() = default;

        public:
            static components::Texture load(const std::string &path);

            static void bind(GLuint textureID);

            static void unbind();
        };
    }
}