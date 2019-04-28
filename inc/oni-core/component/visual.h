#pragma once

#include <utility>
#include <vector>
#include <unordered_map>

#include <oni-core/math/vec2.h>
#include <oni-core/math/vec3.h>
#include <oni-core/math/vec4.h>
#include <oni-core/math/mat4.h>
#include <oni-core/common/typedefs.h>
#include <oni-core/common/typedefs-graphics.h>

namespace oni {
    namespace component {
        // TODO: Switch to integer color. Kinda low prio as most of the time I'll use textured sprites.
        struct Appearance {
            math::vec4 color{0.f, 0.f, 0.f, 0.f};
        };

        struct Trail {
            std::vector<math::vec3> previousPos{};
            std::vector<common::real32> velocity{};
        };

        enum class TextureStatus : common::uint8 {
            INVALID = 0,
            READY = 1,
            NEEDS_LOADING_USING_PATH = 2,
            NEEDS_LOADING_USING_DATA = 3,
            NEEDS_RELOADING_USING_PATH = 4,
            NEEDS_RELOADING_USING_DATA = 5,
        };

        struct Image {
            std::vector<common::uint8> data{};
            common::uint16 width{};
            common::uint16 height{};
        };

        struct Texture {
            Image image{};
            common::oniGLuint textureID{0};
            // GL_BGRA          0x80E1
            common::oniGLenum format{0x80E1};
            // GL_UNSIGNED_BYTE 0x1401
            // GL_FLOAT         0x1406
            common::oniGLenum type{0x1401};
            std::string filePath{};
            std::array<math::vec2, 4> uv{math::vec2{0.f, 0.f}, math::vec2{1.f, 0.f},
                                         math::vec2{1.f, 1.f}, math::vec2{0.f, 1.f}};
            TextureStatus status{TextureStatus::INVALID};
        };

        struct Tessellation {
            common::real32 halfSize{1.f};
        };

        using WorldCoord = math::vec3;

        struct Text {
            // TODO: ordering?
            common::oniGLuint textureID{0};
            math::vec3 position{0.f, 0.f, 0.f};
            std::string textContent{};
            std::vector<size_t> width{};
            std::vector<size_t> height{};
            std::vector<common::uint32> offsetX{};
            std::vector<common::uint32> offsetY{};
            std::vector<common::real32> advanceX{};
            std::vector<common::real32> advanceY{};
            std::vector<math::vec4> uv{};
            common::real32 xScaling{1.f};
            common::real32 yScaling{1.f};
            common::EntityID entityID{0};
        };

        struct PixelRGBA {
            common::uint8 red{0};
            common::uint8 blue{0};
            common::uint8 green{0};
            common::uint8 alpha{0};
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

        enum class ZLayerDef : common::uint8 {
            LAYER_0 = 0,
            LAYER_1 = 1,
            LAYER_2 = 2,
            LAYER_3 = 3,
            LAYER_4 = 4,
            LAYER_5 = 5,
            LAYER_6 = 6,
            LAYER_7 = 7,
            LAYER_8 = 8,
            LAYER_9 = 9,
        };

        using ZLayer = std::unordered_map<component::ZLayerDef, common::real32>;

        enum class BrushType : common::uint8 {
            UNKNOWN,

            PLAIN_RECTANGLE,
            CUSTOM_TEXTURE,

            LAST
        };

        struct Brush {
            BrushType type{BrushType::UNKNOWN};
            math::vec2 size{};
            union {
                PixelRGBA color{};
                const char *textureID;
            };
        };

    }
}