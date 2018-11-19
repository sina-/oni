#pragma once

#include <utility>
#include <vector>

#include <GL/glew.h>

#include <oni-core/math/vec2.h>
#include <oni-core/math/vec3.h>
#include <oni-core/math/vec4.h>
#include <oni-core/math/mat4.h>
#include <oni-core/common/typedefs.h>

namespace oni {
    namespace components {
        // TODO: Switch to integer color. Kinda low prio as most of the time I'll use textured sprites.
        struct Appearance {
            math::vec4 color{0.0f, 0.0f, 0.0f, 0.0f};

            template<class Archive>
            void serialize(Archive &archive) {
                archive(color);
            }
        };

        struct TagColorShaded {
            template<class Archive>
            void serialize(Archive &archive) {}
        };

        struct TagTextureShaded {
            template<class Archive>
            void serialize(Archive &archive) {}
        };

        enum class TextureStatus : common::uint8 {
            READY = 0,
            NEEDS_LOADING_USING_PATH = 1,
            NEEDS_LOADING_USING_DATA = 2,
            NEEDS_RELOADING_USING_PATH = 3,
            NEEDS_RELOADING_USING_DATA = 4,
            INVALID = 5
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
            std::vector<common::uint8> data{};
            TextureStatus status{TextureStatus::INVALID};

            template<class Archive>
            void serialize(Archive &archive) {
                archive(width, height, textureID, format, type, filePath, uv, data, status);
            }
        };

        struct Text {
            // TODO: ordering?
            GLuint textureID{0};
            math::vec3 position{0.0f, 0.0f, 0.0f};
            std::string textContent{};
            std::vector<size_t> width{};
            std::vector<size_t> height{};
            std::vector<common::uint32> offsetX{};
            std::vector<common::uint32> offsetY{};
            std::vector<common::real32> advanceX{};
            std::vector<common::real32> advanceY{};
            std::vector<math::vec4> uv{};
            common::real32 xScaling{1.0f};
            common::real32 yScaling{1.0f};

        };

        struct PixelRGBA {
            unsigned char red{0};
            unsigned char blue{0};
            unsigned char green{0};
            unsigned char alpha{0};
        };

        struct ScreenBounds {
            common::real32 xMin;
            common::real32 xMax;
            common::real32 yMin;
            common::real32 yMax;
        };

        struct Camera {
            common::real32 x;
            common::real32 y;
            common::real32 z;
        };

    }
}