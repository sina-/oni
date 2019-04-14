#include <oni-core/graphic/texture-manager.h>

#include <cassert>

#include <ftgl/texture-atlas.h>
#include <FreeImage.h>
#include <GL/glew.h>

#include <oni-core/graphic/font-manager.h>


namespace oni {
    namespace graphic {

        TextureManager::TextureManager() = default;

        TextureManager::~TextureManager() = default;

        void
        TextureManager::bindRange(common::oniGLuint first,
                                  const std::vector<common::oniGLuint> &textures) {
            if (!textures.empty()) {
                glBindTextures(first, static_cast<common::oniGLsizei>(textures.size()), textures.data());
            }
        }

        const component::Texture &
        TextureManager::loadOrGetTexture(const char *path) {
            if (isTextureLoaded(path)) {
                return mTextureMap[path];
            }
            assert(path);

            const auto &image = loadOrGetImage(path);

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

            std::array<math::vec2, 4> uv{math::vec2{0, 0}, math::vec2{0, 1}, math::vec2{1, 1}, math::vec2{1, 0}};

            auto texture = component::Texture{image, textureID, format, type, path, uv,
                                              component::TextureStatus::READY};
            mTextureMap[path] = texture;

            return mTextureMap[path];
        }

        const component::Image &
        TextureManager::loadOrGetImage(const char *path) {
            assert(path);

            if (isImageLoaded(path)) {
                return mImageMap[path];
            }

            FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
            FIBITMAP *dib = nullptr;

            fif = FreeImage_GetFileType(path, 0);
            if (fif == FIF_UNKNOWN)
                fif = FreeImage_GetFIFFromFilename(path);
            assert(fif != FIF_UNKNOWN);

            if (FreeImage_FIFSupportsReading(fif))
                dib = FreeImage_Load(fif, path);
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

            auto image = component::Image{};
            image.height = height;
            image.width = width;
            image.data.resize(width * height * mElementsInRGBA, 0);

            for (auto i = 0; i < image.data.size(); ++i) {
                image.data[i] = bits[i];
            }

            FreeImage_Unload(dib);

            mImageMap[path] = std::move(image);
            return mImageMap[path];
        }

        component::Image
        TextureManager::generateBits(const common::uint16 width,
                                     const common::uint16 height,
                                     const component::PixelRGBA &pixel) {

            auto image = component::Image{};
            image.width = width;
            image.height = height;
            image.data.insert(image.data.begin(), static_cast<common::uint32>(mElementsInRGBA * width * height), 0);
            common::uint16 stride = width * mElementsInRGBA;

            for (common::uint32 y = 0; y < height; ++y) {
                for (common::uint32 x = 0; x < width; ++x) {
                    image.data[(y * stride) + (x * mElementsInRGBA) + FI_RGBA_BLUE] = pixel.blue;
                    image.data[(y * stride) + (x * mElementsInRGBA) + FI_RGBA_GREEN] = pixel.green;
                    image.data[(y * stride) + (x * mElementsInRGBA) + FI_RGBA_RED] = pixel.red;
                    image.data[(y * stride) + (x * mElementsInRGBA) + FI_RGBA_ALPHA] = pixel.alpha;
                }
            }

            return image;
        }

        void
        TextureManager::updateSubTexture(const component::Texture &texture,
                                         common::oniGLint xOffset,
                                         common::oniGLint yOffset,
                                         common::oniGLint width,
                                         common::oniGLint height,
                                         const std::vector<common::uint8> &bits) {
            assert(texture.image.width > xOffset);
            assert(texture.image.height > yOffset);
            assert(texture.image.width > 0);
            assert(texture.image.height > 0);

            // Clip
            if (xOffset + width >= texture.image.width) {
                width = texture.image.width - xOffset;
            }
            if (yOffset + height >= texture.image.height) {
                height = texture.image.height - yOffset;
            }
            assert(texture.image.width >= xOffset + width);
            assert(texture.image.height >= yOffset + height);

            math::zeroClip(xOffset);
            math::zeroClip(yOffset);

            assert(xOffset >= 0);
            assert(yOffset >= 0);
            assert(texture.image.width >= width);
            assert(texture.image.height >= height);
            glTextureSubImage2D(texture.textureID, 0, xOffset, yOffset, width, height, texture.format, texture.type,
                                bits.data());
        }

        void
        TextureManager::blend(component::Texture &texture,
                              common::oniGLint xOffset,
                              common::oniGLint yOffset,
                              common::oniGLint width,
                              common::oniGLint height,
                              std::vector<common::uint8> &bits) {
            if (xOffset < 0) {
                width += xOffset;
                xOffset = 0;
            }
            if (yOffset < 0) {
                height += yOffset;
                yOffset = 0;
            }

            math::clipUpper(xOffset, texture.image.width - 1);
            math::clipUpper(yOffset, texture.image.height - 1);

            // Clip
            if (xOffset + width >= texture.image.width) {
                width = texture.image.width - xOffset;
            }
            if (yOffset + height >= texture.image.height) {
                height = texture.image.height - yOffset;
            }

            assert(width > 0);
            assert(height > 0);

            assert(texture.image.width >= xOffset + width);
            assert(texture.image.height >= yOffset + height);

            common::uint16 brushStride = width * mElementsInRGBA;
            common::uint16 textureStride = texture.image.width * mElementsInRGBA;

            auto r = FI_RGBA_RED;
            auto g = FI_RGBA_GREEN;
            auto b = FI_RGBA_BLUE;
            auto a = FI_RGBA_ALPHA;

            common::int32 textureOffset = (yOffset * textureStride) + (xOffset * mElementsInRGBA);

            for (common::uint32 y = 0; y < height; ++y) {
                for (common::uint32 x = 0; x < width; ++x) {
                    common::int32 n = (y * brushStride) + (x * mElementsInRGBA);
                    common::int32 m = textureOffset + (y * textureStride) + (x * mElementsInRGBA);

                    assert(m >= 0);
                    assert(m + a < texture.image.data.size());

                    auto oldR = static_cast<common::uint16>(texture.image.data[m + r]) + bits[n + r];
                    auto oldG = static_cast<common::uint16>(texture.image.data[m + g]) + bits[n + g];
                    auto oldB = static_cast<common::uint16>(texture.image.data[m + b]) + bits[n + b];
                    auto oldA = static_cast<common::uint16>(texture.image.data[m + a]) + bits[n + a];

                    math::clipUpper(oldR, 255);
                    math::clipUpper(oldG, 255);
                    math::clipUpper(oldB, 255);
                    math::clipUpper(oldA, 255);

                    bits[n + r] = oldR;
                    bits[n + g] = oldG;
                    bits[n + b] = oldB;
                    bits[n + a] = oldA;

                    texture.image.data[m + r] = bits[n + r];
                    texture.image.data[m + g] = bits[n + g];
                    texture.image.data[m + b] = bits[n + b];
                    texture.image.data[m + a] = bits[n + a];
                }
            }

            updateSubTexture(texture, xOffset, yOffset, width, height, bits);
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
        TextureManager::isTextureLoaded(const char *path) const {
            return mTextureMap.find(path) != mTextureMap.end();
        }

        bool
        TextureManager::isImageLoaded(const char *path) const {
            return mImageMap.find(path) != mImageMap.end();
        }

        void
        TextureManager::bind(common::oniGLuint textureID) {
            glBindTexture(GL_TEXTURE_2D, textureID);
        }

        void
        TextureManager::unbind() {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        component::Texture
        TextureManager::loadFromImage(const component::Image &image) {
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

            std::array<math::vec2, 4> uv{math::vec2{0, 0}, math::vec2{0, 1}, math::vec2{1, 1}, math::vec2{1, 0}};

            // TODO: The path is empty, might be useful to generate a unique path for procgen textures. Maybe with the
            // seed.
            return component::Texture{image, textureID, format, type, "", uv, component::TextureStatus::READY};

        }

        size_t
        TextureManager::numLoadedTexture() const {
            return mTextureMap.size();
        }
    }
}