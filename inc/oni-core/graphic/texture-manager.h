#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <oni-core/component/visual.h>

namespace oni {
    namespace graphic {

        class FontManager;

        class TextureManager {
        public:
            TextureManager();

            ~TextureManager();

            // TODO: This function doesnt fit in the current design
            static void
            bindRange(common::oniGLuint first,
                      const std::vector<common::oniGLuint> &textures);

            static void
            unbind();

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
            void
            updateSubTexture(const component::Texture &texture,
                             common::oniGLint xOffset,
                             common::oniGLint yOffset,
                             common::oniGLint width,
                             common::oniGLint height,
                             const std::vector<common::uint8> &bits);

            void
            blend(component::Texture &texture,
                  common::oniGLint xOffset,
                  common::oniGLint yOffset,
                  common::oniGLint width,
                  common::oniGLint height,
                  std::vector<common::uint8> &bits);

            const component::Texture *
            findOrLoad(const std::string &path);

            static component::Texture
            loadFromData(common::uint16 width,
                         common::uint16 height,
                         const common::TextureData &data);

            // TODO: This function doesnt need to be here, I need a new proc-gen class to handle random
            // data generations of all types
            static common::TextureData
            generateBits(common::uint16 width,
                         common::uint16 height,
                         const component::PixelRGBA &pixel);

            // TODO: This function doesnt fit in the current design
            static common::oniGLuint
            load(const graphic::FontManager &fontManager);

            size_t
            numLoadedTexture() const;

        private:
            bool
            isLoaded(const std::string &path) const;

            const component::Texture *
            load(const std::string &path);

            static void
            bind(common::oniGLuint textureID);


        private:
            std::unordered_map<std::string, component::Texture> mTextureMap{};
        };
    }
}
