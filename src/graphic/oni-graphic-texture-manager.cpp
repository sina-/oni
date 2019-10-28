#include <oni-core/graphic/oni-graphic-texture-manager.h>

#include <cassert>

#include <ftgl/texture-atlas.h>
#include <FreeImage.h>
#include <GL/glew.h>

#include <oni-core/asset/oni-asset-manager.h>
#include <oni-core/graphic/oni-graphic-font-manager.h>
#include <oni-core/math/oni-math-function.h>


namespace oni {
    TextureManager::TextureManager(AssetFilesIndex &assetManager) : mAssetManager(assetManager) {
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
    TextureManager::cacheAllAssets() {
        for (auto iter = mAssetManager.imageAssetsBegin(); iter != mAssetManager.imageAssetsEnd(); ++iter) {
            const auto &imageAsset = iter->second;
            _cacheImage(imageAsset);
            _cacheTexture(imageAsset.name);
        }
    }

    void
    TextureManager::bindRange(oniGLuint first,
                              const std::vector<oniGLuint> &textures) {
        if (!textures.empty()) {
            glBindTextures(first, static_cast<oniGLsizei>(textures.size()), textures.data());
        }
    }

    void
    TextureManager::_initTexture(Texture &texture) {
        glGenTextures(1, &texture.id);

        assert(texture.id);

        // TODO: Hard-coded! This should be part of Texture
        oniGLenum internalFormat = GL_RGBA;
        oniGLenum format = GL_BGRA;
        oniGLenum type = GL_UNSIGNED_BYTE;

        _bind(texture.id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        assert(texture.image.width > 0);
        assert(texture.image.height > 0);

        auto data = (u8 *) nullptr;
        if (texture.image.name != Image::GENERATED) {
            auto imageData = mImageDataMap.find(texture.image.name);
            if (imageData == mImageDataMap.end()) {
                assert(false);
            } else {
                data = imageData->second.data();
            }
        }

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, texture.image.width, texture.image.height, 0, format, type,
                     data);

        texture.format = format;
        texture.type = type;
        texture.uv = {vec2{0, 0}, vec2{0, 1}, vec2{1, 1}, vec2{1, 0}};
        unbind();

    }

    void
    TextureManager::initBlankTexture(Texture &texture) {
        _initTexture(texture);
    }

    void
    TextureManager::initTexture(Texture &texture) {
        assert(texture.image.name.hash.value);
        assert(!texture.image.name.str.empty());
        const auto it = mTextureMap.find(texture.image.name.hash);
        if (it == mTextureMap.end()) {
            // NOTE: Did you forget to call loadAssets()?
            assert(false);
            return;
        }
        // TODO: This is a copy
        texture = it->second;
    }

    void
    TextureManager::_cacheTexture(const AssetName &name) {
        auto newElement = mTextureMap.emplace(name.hash, Texture{});
        if (!newElement.second) {
            assert(false);
            return;
        }
        auto texture = newElement.first;

        _initImage(name, texture->second.image);
        _initTexture(texture->second);
    }

    void
    TextureManager::_initImage(const AssetName &name,
                               Image &image) {
        assert(name.hash.value);

        auto img = mImageMap.find(name.hash);
        if (img != mImageMap.end()) {
            image = img->second;
        } else {
            assert(false);
        }
    }

    void
    TextureManager::_cacheImage(const ImageAsset &asset) {
        assert(asset.name.hash.value);
        assert(!asset.name.str.empty());
        assert(!asset.path.value.empty());
        auto image = mImageMap.find(asset.name.hash);
        if (image != mImageMap.end()) {
            assert(false);
            return;
        }

        FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
        FIBITMAP *dib = nullptr;

        const auto path = asset.path.value.c_str();

        fif = FreeImage_GetFileType(path, 0);
        if (fif == FIF_UNKNOWN) {
            fif = FreeImage_GetFIFFromFilename(path);
        }
        assert(fif != FIF_UNKNOWN);

        if (FreeImage_FIFSupportsReading(fif)) {
            dib = FreeImage_Load(fif, path);
        }
        assert(dib);

        auto colorType = FreeImage_GetColorType(dib);
        // NOTE: This is the only type supported
        assert(colorType == FREE_IMAGE_COLOR_TYPE::FIC_RGBALPHA || colorType == FREE_IMAGE_COLOR_TYPE::FIC_RGB);

        auto width = static_cast<u32>(FreeImage_GetWidth(dib));
        auto height = static_cast<u32 >(FreeImage_GetHeight(dib));

        assert(width);
        assert(height);

        auto result = FreeImage_PreMultiplyWithAlpha(dib);
        assert(result);

        auto bits = FreeImage_GetBits(dib);
        assert(bits);

        auto _image = Image{};
        _image.width = width;
        _image.height = height;
        _image.name = asset.name; // NOTE: this will point name.str to point at what AssetFilesIndex holds
        mImageMap.emplace(asset.name.hash, _image);
        auto &storage = mImageDataMap[asset.name];
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

        _bind(textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, fontManager.getAtlasData());

        unbind();

        return textureID;
    }

    void
    TextureManager::_bind(oniGLuint textureID) {
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    void
    TextureManager::unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
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