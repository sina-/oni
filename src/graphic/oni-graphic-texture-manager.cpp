#include <oni-core/graphic/oni-graphic-texture-manager.h>

#include <cassert>

#include <ftgl/texture-atlas.h>
#include <FreeImage.h>
#include <GL/glew.h>

#include <oni-core/asset/oni-asset-manager.h>
#include <oni-core/graphic/oni-graphic-font-manager.h>
#include <oni-core/math/oni-math-function.h>


namespace oni {
    TextureManager::TextureManager(AssetManager &assetManager) : mAssetManager(assetManager) {
        mAnimationUVs.resize(enumCast(NumAnimationFrames::LAST));
        size numFrames = 0;
        for (auto &&uvs: mAnimationUVs) {
            if (!numFrames) {
                ++numFrames;
                continue;
            }
            uvs.resize(numFrames);
            auto xOffset = 0.f;
            for (auto &&uv: uvs) {
                auto xWidth = 1.f / numFrames;

                uv.values[0] = {xOffset, 0.f};
                uv.values[1] = {xOffset, 1.f};
                uv.values[2] = {xOffset + xWidth, 1.f};
                uv.values[3] = {xOffset + xWidth, 0.f};

                xOffset += xWidth;
            }
            ++numFrames;
        }
    }

    TextureManager::~TextureManager() = default;

    void
    TextureManager::bindRange(oniGLuint first,
                              const std::vector<oniGLuint> &textures) {
        if (!textures.empty()) {
            glBindTextures(first, static_cast<oniGLsizei>(textures.size()), textures.data());
        }
    }

    void
    TextureManager::loadFromTextureID(Texture &texture,
                                      EntityAssetsPack tag) {
        auto numTextureElements = 4;
        auto textureSize = size(texture.image.height * texture.image.width * numTextureElements);
        assert(textureSize);
        auto &storage = mImageDataMap[tag];
        storage.resize(textureSize);
        glBindTexture(GL_TEXTURE_2D, texture.id);
        glGetTextureImage(texture.id, 0, texture.format, texture.type, textureSize,
                          storage.data());
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void
    TextureManager::loadFromTextureID(Texture &texture,
                                      std::vector<u8> &data) {
        auto numTextureElements = 4;
        auto textureSize = size(texture.image.height * texture.image.width * numTextureElements);
        assert(textureSize);
        data.resize(textureSize);
        glBindTexture(GL_TEXTURE_2D, texture.id);
        glGetTextureImage(texture.id, 0, texture.format, texture.type, textureSize, data.data());
        glBindTexture(GL_TEXTURE_2D, 0);

    }

    void
    TextureManager::createTexture(Texture &texture,
                                  EntityAssetsPack tag) {
        glGenTextures(1, &texture.id);

        assert(texture.id);

        // TODO: Hard-coded! This should be part of Texture
        oniGLenum internalFormat = GL_RGBA;
        oniGLenum format = GL_BGRA;
        oniGLenum type = GL_UNSIGNED_BYTE;

        bind(texture.id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        assert(texture.image.width > 0);
        assert(texture.image.height > 0);

        auto data = (u8 *) nullptr;
        if (tag != EntityAssetsPack::GENERATED) {
            assert(mImageDataMap.find(tag) != mImageDataMap.end());
            auto &storage = mImageDataMap[tag];
            data = storage.data();
        }

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, texture.image.width, texture.image.height, 0, format, type,
                     data);

        texture.format = format;
        texture.type = type;
        texture.uv = {vec2{0, 0}, vec2{0, 1}, vec2{1, 1}, vec2{1, 0}};
        unbind();
    }

    const Texture &
    TextureManager::getTexture(EntityAssetsPack tag) {
        assert(tag != EntityAssetsPack::UNKNOWN);
        assert(tag != EntityAssetsPack::LAST);
        const auto it = mTextureMap.find(tag);
        if (it == mTextureMap.end()) {
            assert(false);
        }
        return it->second;
    }

    void
    TextureManager::initTexture(EntityAssetsPack tag,
                                Texture &texture) {
        assert(tag != EntityAssetsPack::UNKNOWN);
        assert(tag != EntityAssetsPack::LAST);
        const auto it = mTextureMap.find(tag);
        if (it == mTextureMap.end()) {
            // NOTE: Did you forget to call loadAssets()?
            assert(false);
            return;
        }
        // TODO: This is a copy
        texture = it->second;
    }

    void
    TextureManager::loadAssets() {
        for (auto &&tag: mAssetManager.knownTags()) {
            loadTextureToCache(tag);
        }
    }

    void
    TextureManager::loadTextureToCache(EntityAssetsPack tag) {
        assert(tag != EntityAssetsPack::UNKNOWN);
        assert(tag != EntityAssetsPack::LAST);
        auto it = mTextureMap.find(tag);
        if (it != mTextureMap.end()) {
            assert(false);
            return;
        }

        auto texture = Texture{};
        loadOrGetImage(tag, texture.image);
        createTexture(texture, tag);
        mTextureMap.insert({tag, texture});
    }

    void
    TextureManager::loadOrGetImage(EntityAssetsPack tag,
                                   Image &image) {
        assert(tag != EntityAssetsPack::UNKNOWN);
        if (isImageLoaded(tag)) {
            image = mImageMap[tag];
        }

        FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
        FIBITMAP *dib = nullptr;

        auto path = mAssetManager.getAssetFilePath(tag);

        fif = FreeImage_GetFileType(path.data(), 0);
        if (fif == FIF_UNKNOWN)
            fif = FreeImage_GetFIFFromFilename(path.data());
        assert(fif != FIF_UNKNOWN);

        if (FreeImage_FIFSupportsReading(fif))
            dib = FreeImage_Load(fif, path.data());
        assert(dib);

        auto colorType = FreeImage_GetColorType(dib);
        // NOTE: This is the only type supported
        assert(colorType == FREE_IMAGE_COLOR_TYPE::FIC_RGBALPHA || colorType == FREE_IMAGE_COLOR_TYPE::FIC_RGB);

        auto width = static_cast<oniGLsizei>(FreeImage_GetWidth(dib));
        auto height = static_cast<oniGLsizei >(FreeImage_GetHeight(dib));

        assert(width);
        assert(height);

        auto result = FreeImage_PreMultiplyWithAlpha(dib);
        assert(result);

        auto bits = FreeImage_GetBits(dib);
        assert(bits);

        image.height = height;
        image.width = width;
        image.path = path;
        auto &storage = mImageDataMap[tag];
        storage.resize(width * height * mElementsInRGBA, 0);

        for (size i = 0; i < storage.size(); ++i) {
            storage[i] = bits[i];
        }

        FreeImage_Unload(dib);
    }

    void
    TextureManager::fill(Image &image,
                         std::vector<u8> &storage,
                         const Color &pixel) {
        assert(image.width);
        assert(image.height);

        storage.resize(static_cast<u32>(mElementsInRGBA * image.width * image.height), 0);
        u16 stride = image.width * mElementsInRGBA;
        if (pixel.value == 0) {
            return;
        }

        for (u32 y = 0; y < image.height; ++y) {
            for (u32 x = 0; x < image.width; ++x) {
                auto r = u8(pixel.r_r32() * pixel.a_r32());
                auto g = u8(pixel.g_r32() * pixel.a_r32());
                auto b = u8(pixel.b_r32() * pixel.a_r32());
                storage[(y * stride) + (x * mElementsInRGBA) + FI_RGBA_RED] = r;
                storage[(y * stride) + (x * mElementsInRGBA) + FI_RGBA_GREEN] = g;
                storage[(y * stride) + (x * mElementsInRGBA) + FI_RGBA_BLUE] = b;
                storage[(y * stride) + (x * mElementsInRGBA) + FI_RGBA_ALPHA] = pixel.a();
            }
        }
    }

    void
    TextureManager::updateSubTexture(const Texture &texture,
                                     oniGLint xOffset,
                                     oniGLint yOffset,
                                     oniGLint width,
                                     oniGLint height,
                                     const std::vector<u8> &bits) {
        assert(texture.image.width > xOffset);
        assert(texture.image.height > yOffset);
        assert(texture.image.width > 0);
        assert(texture.image.height > 0);

        assert(texture.image.width >= xOffset + width);
        assert(texture.image.height >= yOffset + height);

        assert(xOffset >= 0);
        assert(yOffset >= 0);
        assert(texture.image.width >= width);
        assert(texture.image.height >= height);

        // TODO: Do I need ot bind the texture? Seems to be working :/
        glTextureSubImage2D(texture.id, 0, xOffset, yOffset, width, height, texture.format, texture.type,
                            bits.data());
    }

    void
    TextureManager::blendAndUpdateTexture(Texture &texture,
                                          Image &image,
                                          std::vector<u8> &storageTexture,
                                          std::vector<u8> &storageImage,
                                          const vec3 &brushTexturePos) {
        assert(image.width);
        assert(image.height);

        auto xOffset = static_cast<oniGLint>(brushTexturePos.x - (image.width / 2.f));
        auto yOffset = static_cast<oniGLint>(brushTexturePos.y - (image.height / 2.f));

        auto r = FI_RGBA_RED;
        auto g = FI_RGBA_GREEN;
        auto b = FI_RGBA_BLUE;
        auto a = FI_RGBA_ALPHA;

        i32 brushStride = image.width * mElementsInRGBA;
        i32 textureStride = texture.image.width * mElementsInRGBA;

        i32 subImageWidth = image.width;
        i32 subImageHeight = image.height;

        if (yOffset < 0) {
            subImageHeight = image.height + yOffset;
        }
        if (yOffset + image.height > texture.image.height) {
            subImageHeight = texture.image.height - yOffset;
        }

        if (xOffset < 0) {
            subImageWidth = image.width + xOffset;
        }
        if (xOffset + image.width > texture.image.width) {
            subImageWidth = texture.image.width - xOffset;
        }

        if (subImageHeight <= 0 || subImageWidth <= 0) {
            return;
        }

        assert(subImageHeight > 0);
        assert(subImageWidth > 0);

        u32 subImageStride = subImageWidth * mElementsInRGBA;
        std::vector<u8> subImage;
        subImage.resize(subImageHeight * subImageStride, 0);

        for (i32 y = 0; y < image.height; ++y) {
            for (i32 x = 0; x < image.width; ++x) {
                i32 xTexture = x + xOffset;
                i32 yTexture = y + yOffset;

                if (xTexture < 0 || yTexture < 0) {
                    continue;
                }
                if (xTexture > texture.image.width || yTexture > texture.image.height) {
                    continue;
                }

                i32 ySubImage = y;
                i32 xSubImage = x;

                if (yOffset < 0) {
                    ySubImage += yOffset;
                }
                if (xOffset < 0) {
                    xSubImage += xOffset;
                }

                assert(ySubImage >= 0);
                assert(xSubImage >= 0);
                assert(xTexture >= 0);
                assert(yTexture >= 0);

                i32 n = (y * brushStride) + (x * mElementsInRGBA);
                i32 m = (yTexture * textureStride) + (xTexture * mElementsInRGBA);
                i32 p = (ySubImage * subImageStride) + (xSubImage * mElementsInRGBA);

                // TODO: Why does this happen? :/
                if (p + a >= subImage.size()) {
                    continue;
                }

                assert(n + a < storageImage.size());
                assert(m + a < storageTexture.size());
                assert(p + a < subImage.size());

                assert(n >= 0);
                assert(m >= 0);
                assert(p >= 0);

                r32 oldR = storageTexture[m + r] / 255.f;
                r32 oldG = storageTexture[m + g] / 255.f;
                r32 oldB = storageTexture[m + b] / 255.f;
                r32 oldA = storageTexture[m + a] / 255.f;

                r32 newR = storageImage[n + r] / 255.f;
                r32 newG = storageImage[n + g] / 255.f;
                r32 newB = storageImage[n + b] / 255.f;
                r32 newA = storageImage[n + a] / 255.f;

                r32 blendR = lerp(oldR, newR, newA);
                r32 blendG = lerp(oldG, newG, newA);
                r32 blendB = lerp(oldB, newB, newA);
                r32 blendA = newA + oldA;

                clip(blendR, 0.f, 1.f);
                clip(blendG, 0.f, 1.f);
                clip(blendB, 0.f, 1.f);
                clip(blendA, 0.f, 1.f);

                // NOTE: Pre-multiplied alpha
                subImage[p + r] = (u8) (blendR * blendA * 255);
                subImage[p + g] = (u8) (blendG * blendA * 255);
                subImage[p + b] = (u8) (blendB * blendA * 255);
                subImage[p + a] = (u8) (blendA * 255);

                // NOTE: Store the blend for future
                storageTexture[m + r] = subImage[p + r];
                storageTexture[m + g] = subImage[p + g];
                storageTexture[m + b] = subImage[p + b];
                storageTexture[m + a] = subImage[p + a];
            }
        }

        zeroClip(xOffset);
        zeroClip(yOffset);

        updateSubTexture(texture, xOffset, yOffset, subImageWidth, subImageHeight, subImage);
    }

    oniGLuint
    TextureManager::load(const FontManager &fontManager) {
        // TODO: There is no caching in this function!

        auto width = static_cast<oniGLsizei>(fontManager.getAtlasWidth());
        auto height = static_cast<oniGLsizei>(fontManager.getAtlasHeight());

        oniGLuint textureID = 0;
        glGenTextures(1, &textureID);

        assert(textureID);

        oniGLenum internalFormat = fontManager.getAtlasColorFormatInternal();
        oniGLenum format = fontManager.getAtlasColorFormat();
        oniGLenum type = fontManager.getAtlasColorType();

        bind(textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, fontManager.getAtlasData());

        unbind();

        return textureID;
    }

    void
    TextureManager::copy(const Texture &src,
                         Texture &dest) {
    }

    bool
    TextureManager::isTextureLoaded(EntityAssetsPack tag) const {
        return mTextureMap.find(tag) != mTextureMap.end();
    }

    bool
    TextureManager::isImageLoaded(EntityAssetsPack tag) const {
        return mImageMap.find(tag) != mImageMap.end();
    }

    void
    TextureManager::bind(oniGLuint textureID) {
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    void
    TextureManager::unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void
    TextureManager::loadFromData(Texture &texture,
                                 const std::vector<u8> &data) {
        oniGLuint textureID = 0;
        glGenTextures(1, &textureID);

        assert(textureID);

        oniGLint internalFormat = GL_RGBA;
        oniGLenum format = GL_BGRA;
        oniGLenum type = GL_UNSIGNED_BYTE;

        bind(textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, texture.image.width, texture.image.height, 0, format, type,
                     data.data());

        unbind();

        texture.id = textureID;
        texture.format = format;
        texture.type = type;
        texture.uv = {};
    }

    size_t
    TextureManager::numLoadedTexture() const {
        return mTextureMap.size();
    }

    const UV &
    TextureManager::getUV(AnimationID aID,
                          FrameID fID) {
        return mAnimationUVs[aID][fID];
    }

    void
    TextureManager::makeAnim(MaterialTransition_Texture &output,
                             NumAnimationFrames numFrames,
                             r32 fps) {
        output.nextFrame = 0;
        output.playing = true;
        output.numFrames = numFrames;
        output.ttl.maxAge = 1.0 / fps;
        output.nextFrame = 0;
        output.ttl.currentAge = 0;
        output.animID = enumCast(numFrames);
    }
}