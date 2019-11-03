#pragma once

#include <vector>
#include <unordered_map>

#include <oni-core/component/oni-component-visual.h>
#include <oni-core/asset/oni-asset-manager.h>
#include <oni-core/asset/oni-asset-fwd.h>
#include <oni-core/graphic/oni-graphic-fwd.h>


namespace oni {
    class TextureManager {
    public:
        explicit TextureManager(oni::AssetFilesIndex &);

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

        // TODO: I need to refactor this API, not very useful as is. The concept of storage is ugly and brittle
        void
        blendAndUpdateTexture(Texture &texture,
                              Image &image,
                              std::vector<u8> &storageTexture,
                              std::vector<u8> &storageImage,
                              const vec3 &brushTexturePos);

        void
        initTexture(Texture &);

        void
        initBlankTexture(Texture &);

        // TODO: This function doesnt need to be here, I need a new proc-gen class to handle random
        // data generations of all types
        void
        fill(Image &,
             std::vector<u8> &storage,
             const Color &pixel);

        // TODO: This function doesnt fit in the current design
        static oniGLuint
        load(const FontManager &fontManager);

        void
        cacheAllAssets();

        const UV &
        getUV(AnimationID,
              FrameID);

        static void
        makeAnim(MaterialTransition_Texture &,
                 NumAnimationFrames,
                 r32 fps);

    private:
        void
        _cacheImage(const ImageAsset &asset);

        void
        _initImage(const ImageName &,
                   Image &);

        void
        _cacheTexture(const ImageName &);

        void
        _initTexture(Texture &);

        static void
        _bind(oniGLuint textureID);

    private:
        std::unordered_map<Hash, Texture> mTextureMap{};
        std::unordered_map<Hash, Image> mImageMap{};
        std::unordered_map<ImageName, std::vector<u8>> mImageDataMap{};

        const u8 mElementsInRGBA{4};
        oni::AssetFilesIndex &mAssetManager;

        std::vector<std::vector<UV>> mAnimationUVs{};
    };
}
