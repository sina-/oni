#pragma once

#include <string>
#include <vector>
#include <map>

#include <oni-core/component/oni-component-visual.h>
#include <oni-core/asset/oni-asset-manager.h>
#include <oni-core/asset/oni-asset-fwd.h>


namespace oni {

    class FontManager;

    class TextureManager {
    public:
        explicit TextureManager(oni::AssetManager &);

        ~TextureManager();

        // TODO: This function doesnt fit in the current design
        static void
        bindRange(oniGLuint first,
                  const std::vector<oniGLuint> &textures);

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
        updateSubTexture(const Texture &texture,
                         oniGLint xOffset,
                         oniGLint yOffset,
                         oniGLint width,
                         oniGLint height,
                         const std::vector<u8> &bits);

        void
        blendAndUpdateTexture(Texture &texture,
                              Image &image,
                              const vec3 &brushTexturePos);

        void
        loadOrGetImage(EntityPreset tag,
                       Image &image);

        const Texture &
        getTexture(EntityPreset tag);

        void
        initTexture(EntityPreset tag,
                    Texture &texture);

        static void
        createTexture(Texture &texture,
                      bool loadImage);

        static void
        loadFromTextureID(Texture &);

        static void
        loadFromTextureID(Texture &,
                          std::vector<u8> &data);

        static void
        loadFromImage(Texture &);

        // TODO: This function doesnt need to be here, I need a new proc-gen class to handle random
        // data generations of all types
        void
        fill(Image &,
             const Color &pixel);

        // TODO: This function doesnt fit in the current design
        static oniGLuint
        load(const FontManager &fontManager);

        size_t
        numLoadedTexture() const;

        void
        loadAssets();

    private:
        bool
        isTextureLoaded(EntityPreset) const;

        void
        loadTextureToCache(EntityPreset tag);

        bool
        isImageLoaded(EntityPreset) const;

        static void
        bind(oniGLuint textureID);

        static void
        copy(const Texture &,
             Texture &);

    private:

    private:
        std::map<EntityPreset, Texture> mTextureMap{};
        std::map<EntityPreset, Image> mImageMap{};
        const u8 mElementsInRGBA{4};
        oni::AssetManager &mAssetManager;
    };
}
