#include <oni-core/graphic/oni-graphic-texture-manager.h>

#include <cassert>

#include <ftgl/texture-atlas.h>
#include <FreeImage.h>
#include <GL/glew.h>

#include <oni-core/asset/oni-asset-manager.h>
#include <oni-core/graphic/oni-graphic-font-manager.h>


namespace oni {
    namespace graphic {
        TextureManager::TextureManager(asset::AssetManager &assetManager) : mAssetManager(assetManager) {}

        TextureManager::~TextureManager() = default;

        void
        TextureManager::bindRange(common::oniGLuint first,
                                  const std::vector<common::oniGLuint> &textures) {
            if (!textures.empty()) {
                glBindTextures(first, static_cast<common::oniGLsizei>(textures.size()), textures.data());
            }
        }

        const component::Texture &
        TextureManager::loadOrGetTexture(component::TextureTag tag,
                                         bool loadBits) {
            assert(tag != component::TextureTag::UNKNOWN);
            if (isTextureLoaded(tag)) {
                return mTextureMap[tag];
            }
            const auto &image = loadOrGetImage(tag);

            common::oniGLuint textureID = 0;
            glGenTextures(1, &textureID);

            assert(textureID);

            common::oniGLenum format = GL_BGRA;
            common::oniGLenum type = GL_UNSIGNED_BYTE;

            bind(textureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, format, type, image.data.data());

            unbind();

            auto &texture = mTextureMap[tag];
            texture.image = image;
            texture.textureID = textureID;
            texture.format = format;
            texture.type = type;
            texture.uv = {math::vec2{0, 0}, math::vec2{0, 1}, math::vec2{1, 1}, math::vec2{1, 0}};

            // TODO: This is clunky design, these bits are already copied, have to think about a way to
            // split between texture data that is generated and data that is loaded from the file, for the
            // ones loaded from the file I don't need to keep a copy in the memory as I just have them in video memory
            // but for generated ones I keep them in the memory as I often need to blit onto it, like Canvas tiles.
            if (!loadBits) {
                texture.image.data.clear();
            }

            return texture;
        }

        const component::Image &
        TextureManager::loadOrGetImage(component::TextureTag tag) {
            assert(tag != component::TextureTag::UNKNOWN);
            if (isImageLoaded(tag)) {
                return mImageMap[tag];
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
            assert(colorType == FREE_IMAGE_COLOR_TYPE::FIC_RGBALPHA);

            auto width = static_cast<common::oniGLsizei>(FreeImage_GetWidth(dib));
            auto height = static_cast<common::oniGLsizei >(FreeImage_GetHeight(dib));

            assert(width);
            assert(height);

            auto bits = FreeImage_GetBits(dib);
            assert(bits);

            auto &image = mImageMap[tag];
            image.height = height;
            image.width = width;
            image.path = path;
            image.data.resize(width * height * mElementsInRGBA, 0);

            for (auto i = 0; i < image.data.size(); ++i) {
                image.data[i] = bits[i];
            }

            FreeImage_Unload(dib);

            return image;
        }

        void
        TextureManager::fill(component::Image &image,
                             const component::PixelRGBA &pixel) {
            assert(image.width);
            assert(image.height);

            image.width = image.width;
            image.height = image.height;
            image.data.resize(static_cast<common::u32>(mElementsInRGBA * image.width * image.height), 0);
            common::u16 stride = image.width * mElementsInRGBA;

            for (common::u32 y = 0; y < image.height; ++y) {
                for (common::u32 x = 0; x < image.width; ++x) {
                    image.data[(y * stride) + (x * mElementsInRGBA) + FI_RGBA_BLUE] = pixel.blue;
                    image.data[(y * stride) + (x * mElementsInRGBA) + FI_RGBA_GREEN] = pixel.green;
                    image.data[(y * stride) + (x * mElementsInRGBA) + FI_RGBA_RED] = pixel.red;
                    image.data[(y * stride) + (x * mElementsInRGBA) + FI_RGBA_ALPHA] = pixel.alpha;
                }
            }
        }

        void
        TextureManager::updateSubTexture(const component::Texture &texture,
                                         common::oniGLint xOffset,
                                         common::oniGLint yOffset,
                                         common::oniGLint width,
                                         common::oniGLint height,
                                         const std::vector<common::u8> &bits) {
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

            glTextureSubImage2D(texture.textureID, 0, xOffset, yOffset, width, height, texture.format, texture.type,
                                bits.data());
        }

        void
        TextureManager::blendAndUpdateTexture(component::Texture &texture,
                                              common::oniGLint xOffset,
                                              common::oniGLint yOffset,
                                              component::Image &image) {
            auto r = FI_RGBA_RED;
            auto g = FI_RGBA_GREEN;
            auto b = FI_RGBA_BLUE;
            auto a = FI_RGBA_ALPHA;

            common::i32 brushStride = image.width * mElementsInRGBA;
            common::i32 textureStride = texture.image.width * mElementsInRGBA;

            common::i32 subImageWidth = image.width;
            common::i32 subImageHeight = image.height;

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

            common::u32 subImageStride = subImageWidth * mElementsInRGBA;
            std::vector<common::u8> subImage;
            subImage.resize(subImageHeight * subImageStride, 0);

            for (common::i32 y = 0; y < image.height; ++y) {
                for (common::i32 x = 0; x < image.width; ++x) {
                    common::i32 xTexture = x + xOffset;
                    common::i32 yTexture = y + yOffset;

                    if (xTexture < 0 || yTexture < 0) {
                        continue;
                    }
                    if (xTexture > texture.image.width || yTexture > texture.image.height) {
                        continue;
                    }

                    common::i32 ySubImage = y;
                    common::i32 xSubImage = x;

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

                    common::i32 n = (y * brushStride) + (x * mElementsInRGBA);
                    common::i32 m = (yTexture * textureStride) + (xTexture * mElementsInRGBA);
                    common::i32 p = (ySubImage * subImageStride) + (xSubImage * mElementsInRGBA);

                    // TODO: Why does this happen? :/
                    if (p + a >= subImage.size()) {
                        continue;
                    }

                    assert(n + a < image.data.size());
                    assert(m + a < texture.image.data.size());
                    assert(p + a < subImage.size());

                    assert(n >= 0);
                    assert(m >= 0);
                    assert(p >= 0);

                    common::r32 oldR = texture.image.data[m + r] / 255.f;
                    common::r32 oldG = texture.image.data[m + g] / 255.f;
                    common::r32 oldB = texture.image.data[m + b] / 255.f;
                    common::r32 oldA = texture.image.data[m + a] / 255.f;

                    common::r32 newR = image.data[n + r] / 255.f;
                    common::r32 newG = image.data[n + g] / 255.f;
                    common::r32 newB = image.data[n + b] / 255.f;
                    common::r32 newA = image.data[n + a] / 255.f;

                    common::r32 blendR = math::lerp(oldR, newR, newA);
                    common::r32 blendG = math::lerp(oldG, newG, newA);
                    common::r32 blendB = math::lerp(oldB, newB, newA);
                    common::r32 blendA = newA + oldA;

                    math::clip(blendR, 0.f, 1.f);
                    math::clip(blendG, 0.f, 1.f);
                    math::clip(blendB, 0.f, 1.f);
                    math::clip(blendA, 0.f, 1.f);

                    subImage[p + r] = (common::u8) (blendR * 255);
                    subImage[p + g] = (common::u8) (blendG * 255);
                    subImage[p + b] = (common::u8) (blendB * 255);
                    subImage[p + a] = (common::u8) (blendA * 255);

                    // NOTE: Store the blend for future
                    texture.image.data[m + r] = (common::u8) (blendR * 255);
                    texture.image.data[m + g] = (common::u8) (blendG * 255);
                    texture.image.data[m + b] = (common::u8) (blendB * 255);
                    texture.image.data[m + a] = (common::u8) (blendA * 255);
                }
            }

            math::zeroClip(xOffset);
            math::zeroClip(yOffset);

            updateSubTexture(texture, xOffset, yOffset, subImageWidth, subImageHeight, subImage);
        }

        common::oniGLuint
        TextureManager::load(const graphic::FontManager &fontManager) {
            // TODO: There is no caching in this function!

            auto width = static_cast<common::oniGLsizei>(fontManager.getAtlasWidth());
            auto height = static_cast<common::oniGLsizei>(fontManager.getAtlasHeight());

            common::oniGLuint textureID = 0;
            glGenTextures(1, &textureID);

            assert(textureID);

            common::oniGLint internalFormat = GL_RED;
            common::oniGLenum format = GL_RED;
            common::oniGLenum type = GL_UNSIGNED_BYTE;

            bind(textureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, fontManager.getAtlasData());

            unbind();

            return textureID;
        }


        bool
        TextureManager::isTextureLoaded(component::TextureTag tag) const {
            return mTextureMap.find(tag) != mTextureMap.end();
        }

        bool
        TextureManager::isImageLoaded(component::TextureTag tag) const {
            return mImageMap.find(tag) != mImageMap.end();
        }

        void
        TextureManager::bind(common::oniGLuint textureID) {
            glBindTexture(GL_TEXTURE_2D, textureID);
        }

        void
        TextureManager::unbind() {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        void
        TextureManager::loadFromImage(component::Texture &texture) {
            common::oniGLuint textureID = 0;
            glGenTextures(1, &textureID);

            assert(textureID);

            common::oniGLenum format = GL_BGRA;
            common::oniGLenum type = GL_UNSIGNED_BYTE;

            bind(textureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.image.width, texture.image.height, 0, format, type,
                         texture.image.data.data());

            unbind();

            std::array<math::vec2, 4> uv{math::vec2{0, 0}, math::vec2{0, 1}, math::vec2{1, 1}, math::vec2{1, 0}};

            texture.textureID = textureID;
            texture.format = format;
            texture.type = type;
            texture.uv = uv;
        }

        size_t
        TextureManager::numLoadedTexture() const {
            return mTextureMap.size();
        }
    }
}