#pragma once

// TODO: remove memory from this file
#include <memory>
#include <utility>
#include <vector>

#include <GL/glew.h>

#include <oni-core/math/vec2.h>
#include <oni-core/math/vec3.h>
#include <oni-core/math/vec4.h>
#include <oni-core/math/mat4.h>

namespace oni {
    namespace components {
        // TODO: Switch to integer color. Kinda low prio as most of the time I'll use textured sprites.
        struct Appearance {
            math::vec4 color{0.0f, 0.0f, 0.0f, 0.0f};
        };

        struct TagColorShaded {
        };

        struct TagTextureShaded {
        };

        struct Texture {
            // TODO: This might need re ordering for better caching.
            GLsizei width{0};
            GLsizei height{0};
            GLuint textureID{0};
            GLenum format{GL_BGRA};
            GLenum type{GL_UNSIGNED_BYTE};
            std::string filePath{};
            std::vector<math::vec2> uv{math::vec2{0.0f, 0.0f}, math::vec2{1.0f, 0.0f},
                                       math::vec2{1.0f, 1.0f}, math::vec2{0.0f, 1.0f}};
        };

        struct Text {
            // TODO: ordering?
            GLuint textureID{0};
            math::vec3 position{0.0f, 0.0f, 0.0f};
            std::string textContent{};
            std::vector<size_t> width{};
            std::vector<size_t> height{};
            std::vector<int> offsetX{};
            std::vector<int> offsetY{};
            std::vector<float> advanceX{};
            std::vector<float> advanceY{};
            std::vector<math::vec4> uv{};
            float xScaling{1.0f};
            float yScaling{1.0f};

        };

        struct PixelRGBA {
            unsigned char red{0};
            unsigned char blue{0};
            unsigned char green{0};
            unsigned char alpha{0};
        };

        struct ScreenBounds {
            float xMin;
            float xMax;
            float yMin;
            float yMax;
        };

    }
}