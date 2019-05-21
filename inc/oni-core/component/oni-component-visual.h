#pragma once

#include <utility>
#include <vector>
#include <unordered_map>

#include <oni-core/math/oni-math-vec2.h>
#include <oni-core/math/oni-math-vec3.h>
#include <oni-core/math/oni-math-vec4.h>
#include <oni-core/math/oni-math-mat4.h>
#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/common/oni-common-typedefs-graphic.h>
#include <oni-core/component/oni-component-geometry.h>

namespace oni {
    namespace component {
        struct Trail {
            std::vector<component::WorldP3D> previousPos{};
            std::vector<common::r32> velocity{};
        };

        enum class TextureStatus : common::u8 {
            INVALID = 0,
            READY = 1,
            NEEDS_LOADING_USING_PATH = 2,
            NEEDS_LOADING_USING_DATA = 3,
            NEEDS_RELOADING_USING_PATH = 4,
            NEEDS_RELOADING_USING_DATA = 5,
        };

        struct Image {
            common::u16 width{};
            common::u16 height{};
            std::vector<common::u8> data{};
        };

        struct Texture {
            Image image{};
            common::oniGLuint textureID{0};
            // GL_BGRA          0x80E1
            common::oniGLenum format{0x80E1};
            // GL_UNSIGNED_BYTE 0x1401
            // GL_FLOAT         0x1406
            common::oniGLenum type{0x1401};
            std::string path{};
            std::array<math::vec2, 4> uv{math::vec2{0.f, 0.f}, math::vec2{1.f, 0.f},
                                         math::vec2{1.f, 1.f}, math::vec2{0.f, 1.f}};
            TextureStatus status{TextureStatus::NEEDS_LOADING_USING_PATH};
        };

        struct Text {
            common::EntityID entityID{0};
            common::r32 xScaling{1.f};
            common::r32 yScaling{1.f};
            common::oniGLuint textureID{0};
            std::string textContent{};
            std::vector<size_t> width{};
            std::vector<size_t> height{};
            std::vector<common::u32> offsetX{};
            std::vector<common::u32> offsetY{};
            std::vector<common::r32> advanceX{};
            std::vector<common::r32> advanceY{};
            std::vector<math::vec4> uv{};
        };

        struct PixelRGBA {
            common::u8 red{0};
            common::u8 blue{0};
            common::u8 green{0};
            common::u8 alpha{0};
        };

        // TODO: Switch to integer color. Kinda low prio as most of the time I'll use textured sprites.
        struct Appearance {
            math::vec4 color{0.f, 0.f, 0.f, 0.f};

            PixelRGBA
            toRGBA() {
                return PixelRGBA{static_cast<common::u8>(color.x * 255),
                                 static_cast<common::u8>(color.y * 255),
                                 static_cast<common::u8>(color.z * 255),
                                 static_cast<common::u8>(color.w * 255)};
            }
        };

        enum class BrushType : common::u8 {
            UNKNOWN,

            SPRITE,
            TEXTURE,

            LAST
        };

        struct SmokeEmitterCD {
            common::r64 currentCD{0.f};
            common::r64 initialCD{1.f};
        };
    }
}