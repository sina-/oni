#pragma once

#include <string>
#include <vector>
#include <map>

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
            static void
            updateSubTexture(const component::Texture &texture,
                             common::oniGLint xOffset,
                             common::oniGLint yOffset,
                             common::oniGLint width,
                             common::oniGLint height,
                             const std::vector<common::u8> &bits);

            void
            blendAndUpdateTexture(component::Texture &texture,
                                  component::Image &image,
                                  const math::vec3 &brushTexturePos);

            void
            loadOrGetImage(component::EntityPreset tag,
                           component::Image &image);

            const component::Texture &
            getTexture(component::EntityPreset tag);

            void
            initTexture(component::EntityPreset tag,
                        component::Texture &texture);

            static void
            createTexture(component::Texture &texture,
                          bool loadImage);

            static void
            loadFromTextureID(component::Texture &);

            static void
            loadFromTextureID(component::Texture &,
                              std::vector<common::u8> &data);

            static void
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

            void
            loadAssets();

        private:
            bool
            isTextureLoaded(component::EntityPreset) const;

            void
            loadTextureToCache(component::EntityPreset tag);

            bool
            isImageLoaded(component::EntityPreset) const;

            static void
            bind(common::oniGLuint textureID);

            static void
            copy(const component::Texture &,
                 component::Texture &);

        private:

        private:
            std::map<component::EntityPreset, component::Texture> mTextureMap{};
            std::map<component::EntityPreset, component::Image> mImageMap{};
            const common::u8 mElementsInRGBA{4};
            asset::AssetManager &mAssetManager;
        };
    }
}
