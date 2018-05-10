#pragma once

#include <string>
#include <vector>

#include <GL/glew.h>
#include <oni-core/common/typedefs.h>

namespace oni {
    namespace components {
        class Texture;

        class PixelRGBA;
    }
    namespace graphics {
        class FontManager;

        class Texture {
            Texture() = default;

            ~Texture() = default;

        public:
            // TODO: At some point would be nice to have a list of all loaded textures and only load a new
            // one if it has not been loaded already. Perhaps its easier to have a texture manager just like
            // font manager or audio manager.
            static components::Texture load(const std::string &path);

            static components::Texture generate(const common::uint16 width, const common::uint16 height, const components::PixelRGBA &pixel);

            static std::vector<unsigned char> generateBits(const common::uint16 width, const common::uint16 height,
                                                           const components::PixelRGBA &pixel);

            /**
             * Given a texture object and a region in the local texture coordinates overwrites
             * the texture data with the new bits.
             *
             * @param texture
             * @param xOffset in local texture coordinates
             * @param yOffset in local texture coordinates
             * @param width in local texture coordinates
             * @param height in local texture coordinates
             * @param bits data to overwrite the texture with, must match the given region
             */
            static void updateSubTexture(components::Texture texture, const GLint xOffset,
                                         const GLint yOffset, const GLint width,
                                         const GLint height,
                                         const std::vector<unsigned char> &bits);

            static GLuint load(const graphics::FontManager &fontManager);

            static void bind(GLuint textureID);

            static void unbind();
        };
    }
}