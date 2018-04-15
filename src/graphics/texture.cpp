#include <oni-core/graphics/texture.h>

#include <ftgl/texture-atlas.h>
#include <FreeImage.h>

#include <oni-core/entities/basic-entity-repo.h>
#include <oni-core/graphics/font-manager.h>
#include <oni-core/io/output.h>
#include <oni-core/utils/oni-assert.h>

namespace oni {
    namespace graphics {
        components::Texture Texture::load(const std::string &path) {
            FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
            FIBITMAP *dib = nullptr;

            fif = FreeImage_GetFileType(path.c_str(), 0);
            if (fif == FIF_UNKNOWN)
                fif = FreeImage_GetFIFFromFilename(path.c_str());
            if (fif == FIF_UNKNOWN) {
                std::runtime_error("Could not determine image type: " + path);
            }

            if (FreeImage_FIFSupportsReading(fif))
                dib = FreeImage_Load(fif, path.c_str());
            if (!dib) {
                std::runtime_error("Could load image: " + path);
            }

            auto bits = FreeImage_GetBits(dib);
            auto width = FreeImage_GetWidth(dib);
            auto height = FreeImage_GetHeight(dib);

            if ((bits == nullptr) || (width == 0) || (height == 0)) {
                std::runtime_error("Image loaded with no data: " + path);
            }

            GLuint textureID = 0;
            glGenTextures(1, &textureID);

            if (!textureID) {
                throw std::runtime_error("Could not generate texture.");
            }

            GLenum format = GL_BGRA;
            GLenum type = GL_UNSIGNED_BYTE;

            bind(textureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, type, bits);

            unbind();

            FreeImage_Unload(dib);

            std::vector<math::vec2> uv{math::vec2(0, 0), math::vec2(0, 1), math::vec2(1, 1), math::vec2(1, 0)};

            return components::Texture(path, textureID, width, height, format, type, uv);
        }

        void Texture::bind(GLuint textureID) {
            glBindTexture(GL_TEXTURE_2D, textureID);
        }

        void Texture::unbind() {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        GLuint Texture::load(const graphics::FontManager &fontManager) {
            auto width = fontManager.getAtlasWidth();
            auto height = fontManager.getAtlasHeight();

            GLuint textureID = 0;
            glGenTextures(1, &textureID);

            if (!textureID) {
                throw std::runtime_error("Could not generate texture.");
            }

            GLenum format = GL_RED;
            GLenum type = GL_UNSIGNED_BYTE;

            bind(textureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, fontManager.getAtlasData());

            unbind();

            return textureID;
        }

        components::Texture Texture::generate(const int width, const int height, const components::PixelRGBA &pixel) {
            auto bits = generateBits(width, height, pixel);

            GLuint textureID = 0;
            glGenTextures(1, &textureID);

            if (!textureID) {
                throw std::runtime_error("Could not generate texture.");
            }

            GLenum format = GL_BGRA;
            GLenum type = GL_UNSIGNED_BYTE;

            Texture::bind(textureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, type, bits.data());

            Texture::unbind();

            std::vector<math::vec2> uv{math::vec2(0, 0), math::vec2(0, 1), math::vec2(1, 1), math::vec2(1, 0)};

            return components::Texture("", textureID, width, height, format, type, uv);
        }

        void Texture::updateSubTexture(components::Texture texture, const GLint xOffset, const GLint yOffset,
                                       const GLint width, const GLint height,
                                       const std::vector<unsigned char> &bits) {
            ONI_DEBUG_ASSERT(texture.width > xOffset);
            ONI_DEBUG_ASSERT(texture.height > yOffset);
            ONI_DEBUG_ASSERT(xOffset >= 0);
            ONI_DEBUG_ASSERT(yOffset >= 0);
            ONI_DEBUG_ASSERT(texture.width >= width);
            ONI_DEBUG_ASSERT(texture.height >= height);
            glTextureSubImage2D(texture.textureID, 0, xOffset, yOffset, width, height, texture.format, texture.type,
                                bits.data());
        }

        std::vector<unsigned char> Texture::generateBits(const int width, const int height,
                                                         const components::PixelRGBA &pixel) {
            std::vector<unsigned char> bits(4 * width * height, 0);
            // Elements in pixel
            auto eip = 4;
            auto stride = width * eip;

            for (auto y = 0; y < height; ++y) {
                for (auto x = 0; x < width; ++x) {
                    bits[(y * stride) + (x * eip) + FI_RGBA_BLUE] = pixel.blue;
                    bits[(y * stride) + (x * eip) + FI_RGBA_GREEN] = pixel.green;
                    bits[(y * stride) + (x * eip) + FI_RGBA_RED] = pixel.red;
                    bits[(y * stride) + (x * eip) + FI_RGBA_ALPHA] = pixel.alpha;
                }
            }

            return bits;
        }
    }
}