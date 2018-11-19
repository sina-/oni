#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <GL/glew.h>

#include <oni-core/components/visual.h>

namespace oni {
    namespace graphics {

        class FontManager;

        class TextureManager {
        public:
            TextureManager();

            ~TextureManager();

            // TODO: This function doesnt fit in the current design
            static void bindRange(GLuint first, const std::vector<GLuint> &textures);

            static void unbind();

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
            void updateSubTexture(const components::Texture &texture, GLint xOffset,
                                  GLint yOffset, GLint width,
                                  GLint height,
                                  const std::vector<common::uint8> &bits);

            const components::Texture *findOrLoad(const std::string &path);

            static components::Texture loadFromData(common::uint16 width, common::uint16 height,
                                                    const std::vector<common::uint8> &data);

            // TODO: This function doesnt need to be here, I need a new proc-gen class to handle random
            // data generations of all types
            static std::vector<common::uint8> generateBits(common::uint16 width, common::uint16 height,
                                                           const components::PixelRGBA &pixel);

            // TODO: This function doesnt fit in the current design
            static GLuint load(const graphics::FontManager &fontManager);

            size_t numLoadedTexture() const;

        private:
            bool isLoaded(const std::string &path) const;

            const components::Texture *load(const std::string &path);

            static void bind(GLuint textureID);


        private:
            std::unordered_map<std::string, components::Texture> mTextureMap{};
        };
    }
}
