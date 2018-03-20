#pragma once

#include <GL/glew.h>
#include <string>
#include <entities/world.h>
#include <ftgl/texture-atlas.h>
#include <graphics/font-manager.h>

namespace oni {
    namespace graphics {
        class LoadTexture {
            LoadTexture() = default;

            ~LoadTexture() = default;

        public:
            // TODO: At some point would be nice to have a list of all loaded textures and only load a new
            // one if it has not been loaded already. Perhaps its easier to have a texture manager just like
            // font manager or audio manager.
            static components::Texture load(const std::string &path);

            static GLuint load(const graphics::FontManager &fontManager);

            static void bind(GLuint textureID);

            static void unbind();
        };
    }
}