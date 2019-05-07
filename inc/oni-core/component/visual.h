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
#include <oni-core/component/geometry.h>

namespace oni {
    namespace component {
        struct Trail {
            std::vector<component::WorldP3D> previousPos{};
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

        struct Text {
            common::EntityID entityID{0};
            common::real32 xScaling{1.f};
            common::real32 yScaling{1.f};
            common::oniGLuint textureID{0};
            std::string textContent{};
            std::vector<size_t> width{};
            std::vector<size_t> height{};
            std::vector<common::uint32> offsetX{};
            std::vector<common::uint32> offsetY{};
            std::vector<common::real32> advanceX{};
            std::vector<common::real32> advanceY{};
            std::vector<math::vec4> uv{};
        };

        struct PixelRGBA {
            common::uint8 red{0};
            common::uint8 blue{0};
            common::uint8 green{0};
            common::uint8 alpha{0};
        };

        // TODO: Switch to integer color. Kinda low prio as most of the time I'll use textured sprites.
        struct Appearance {
            math::vec4 color{0.f, 0.f, 0.f, 0.f};

            PixelRGBA
            toRGBA() {
                return PixelRGBA{static_cast<common::uint8>(color.x * 255),
                                 static_cast<common::uint8>(color.y * 255),
                                 static_cast<common::uint8>(color.z * 255),
                                 static_cast<common::uint8>(color.w * 255)};
            }
        };


        enum class BrushType : common::uint8 {
            UNKNOWN,

            SPRITE,
            TEXTURE,

            LAST
        };

        // TODO: This is a good example to start thinking about what should be considered a component. Does it make sense
        // for every entity to have Brush component? What is the expected behaviour from it? I could think of it as a
        // component that can convert any entity to splat, then I can remove the color and textureID as those would be
        // required to have and I just have a size. Otherwise, if not every entity can have this component then it is
        // just a data structure used by systems and shouldn't be here.
        struct Brush {
            BrushType type{BrushType::UNKNOWN};
            union {
                PixelRGBA color{};
                const char *textureID;
            };
        };
    }
}