#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <oni-core/component/oni-component-visual.h>

namespace oni {
    namespace asset {
        class AssetManager;
    }
    namespace graphic {

        class FontManager;

        class TextureManager {
        public:
            explicit TextureManager(asset::AssetManager &);

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
                             const std::vector<common::u8> &bits);

            void
            blendAndUpdateTexture(component::Texture &texture,
                                  common::oniGLint xOffset,
                                  common::oniGLint yOffset,
                                  component::Image &image);

            const component::Image &
            loadOrGetImage(component::TextureTag tag);

            const component::Texture &
            loadOrGetTexture(component::TextureTag tag,
                             bool loadBits);

            void
            loadFromImage(component::Texture &);

            // TODO: This function doesnt need to be here, I need a new proc-gen class to handle random
            // data generations of all types
            void
            fill(component::Image &,
                 const component::Color &pixel);

            // TODO: This function doesnt fit in the current design
            static common::oniGLuint
            load(const graphic::FontManager &fontManager);

            size_t
            numLoadedTexture() const;

        private:
            bool
            isTextureLoaded(component::TextureTag) const;

            bool
            isImageLoaded(component::TextureTag) const;

            static void
            bind(common::oniGLuint textureID);

        private:

        private:
            std::unordered_map<component::TextureTag, component::Texture> mTextureMap{};
            std::unordered_map<component::TextureTag, component::Image> mImageMap{};
            const common::u8 mElementsInRGBA{4};
            asset::AssetManager &mAssetManager;
        };
    }
}
