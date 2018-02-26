#include <graphics/texture.h>
#include <FreeImage.h>

namespace oni {
    namespace graphics {
        components::Texture LoadTexture::load(const std::string &path) {
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

            bind(textureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, bits);

            unbind();

            // TODO: Check for errors on loading texture

            FreeImage_Unload(dib);

            return components::Texture(path, textureID, width, height);
        }

        void LoadTexture::bind(GLuint textureID) {
            glBindTexture(GL_TEXTURE_2D, textureID);
        }

        void LoadTexture::unbind() {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
}