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

        void TextureManager::bindRange(common::oniGLuint first, const std::vector<common::oniGLuint> &textures) {
            if (!textures.empty()) {
                glBindTextures(first, static_cast<common::oniGLsizei>(textures.size()), textures.data());
            }
        }

        const component::Texture *TextureManager::findOrLoad(const std::string &path) {
            if (isLoaded(path)) {
                return &mTextureMap[path];
            }
            auto texture = load(path);
            return texture;
        }

        const component::Texture *TextureManager::load(const std::string &path) {
            assert(!path.empty());

            if (isLoaded(path)) {
                return &mTextureMap[path];
            }

            FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
            FIBITMAP *dib = nullptr;

            fif = FreeImage_GetFileType(path.c_str(), 0);
            if (fif == FIF_UNKNOWN)
                fif = FreeImage_GetFIFFromFilename(path.c_str());
            if (fif == FIF_UNKNOWN) {
                throw std::runtime_error("Could not determine image type: " + path);
            }

            if (FreeImage_FIFSupportsReading(fif))
                dib = FreeImage_Load(fif, path.c_str());
            if (!dib) {
                throw std::runtime_error("Could load image: " + path);
            }

            auto bits = FreeImage_GetBits(dib);
            auto width = static_cast<common::oniGLsizei>(FreeImage_GetWidth(dib));
            auto height = static_cast<common::oniGLsizei >(FreeImage_GetHeight(dib));

            if ((bits == nullptr) || (width == 0) || (height == 0)) {
                throw std::runtime_error("Image loaded with no data: " + path);
            }

            common::oniGLuint textureID = 0;
            glGenTextures(1, &textureID);

            if (!textureID) {
                throw std::runtime_error("Could not generate texture.");
            }

            common::oniGLenum format = GL_BGRA;
            common::oniGLenum type = GL_UNSIGNED_BYTE;

            bind(textureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, type, bits);

            unbind();

            FreeImage_Unload(dib);

            std::array<math::vec2, 4> uv{math::vec2{0, 0}, math::vec2{0, 1}, math::vec2{1, 1}, math::vec2{1, 0}};

            auto texture = component::Texture{width, height, textureID, format, type, path, uv, {},
                                              component::TextureStatus::READY};
            mTextureMap[path] = texture;

            return &mTextureMap[path];
        }

        common::TextureData TextureManager::generateBits(const common::uint16 width, const common::uint16 height,
                                                         const component::PixelRGBA &pixel) {
            // Elements in pixel
            common::uint8 eip = 4;
            common::TextureData bits(static_cast<common::uint32>(eip * width * height), 0);
            common::uint16 stride = width * eip;

            for (common::uint32 y = 0; y < height; ++y) {
                for (common::uint32 x = 0; x < width; ++x) {
                    bits[(y * stride) + (x * eip) + FI_RGBA_BLUE] = pixel.blue;
                    bits[(y * stride) + (x * eip) + FI_RGBA_GREEN] = pixel.green;
                    bits[(y * stride) + (x * eip) + FI_RGBA_RED] = pixel.red;
                    bits[(y * stride) + (x * eip) + FI_RGBA_ALPHA] = pixel.alpha;
                }
            }

            return bits;
        }

        void TextureManager::updateSubTexture(const component::Texture &texture,
                                              const common::oniGLint xOffset, const common::oniGLint yOffset,
                                              common::oniGLint width, common::oniGLint height,
                                              const std::vector<common::uint8> &bits) {
            assert(texture.width > xOffset);
            assert(texture.height > yOffset);

            // Truncate
            if (xOffset + width >= texture.width) {
                width = texture.width - xOffset - 1;
            }
            if (yOffset + height >= texture.height) {
                height = texture.height - yOffset - 1;
            }
            assert(texture.width > xOffset + width);
            assert(texture.height > yOffset + height);
            if (xOffset < 0 || yOffset < 0) {
                return;
            }
            assert(xOffset >= 0);
            assert(yOffset >= 0);
            assert(texture.width >= width);
            assert(texture.height >= height);
            glTextureSubImage2D(texture.textureID, 0, xOffset, yOffset, width, height, texture.format, texture.type,
                                bits.data());
        }

        common::oniGLuint TextureManager::load(const graphic::FontManager &fontManager) {
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


        bool TextureManager::isLoaded(const std::string &path) const {
            return mTextureMap.find(path) != mTextureMap.end();
        }

        void TextureManager::bind(common::oniGLuint textureID) {
            glBindTexture(GL_TEXTURE_2D, textureID);
        }

        void TextureManager::unbind() {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        component::Texture TextureManager::loadFromData(common::uint16 width, common::uint16 height,
                                                        const common::TextureData &data) {
            common::oniGLuint textureID = 0;
            glGenTextures(1, &textureID);

            assert(textureID);

            common::oniGLenum format = GL_BGRA;
            common::oniGLenum type = GL_UNSIGNED_BYTE;

            bind(textureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, type, data.data());

            unbind();

            std::array<math::vec2, 4> uv{math::vec2{0, 0}, math::vec2{0, 1}, math::vec2{1, 1}, math::vec2{1, 0}};

            // TODO: The path is empty, might be useful to generate a unique path for procgen textures. Maybe with the
            // seed.
            return component::Texture{width, height, textureID, format, type, "", uv, data,
                                      component::TextureStatus::READY};

        }

        size_t TextureManager::numLoadedTexture() const {
            return mTextureMap.size();
        }
    }
}