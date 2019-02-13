#pragma once

#include <utility>
#include <vector>

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

        struct Particle {
            common::real32 life{0.f};
            math::vec3 pos{0.f, 0.f, 0.f};
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
            common::oniGLsizei width{0};
            common::oniGLsizei height{0};
            common::oniGLuint textureID{0};
            // GL_BGRA
            common::oniGLenum format{0x80E1};
            // GL_UNSIGNED_BYTE
            common::oniGLenum type{0x1401};
            std::string filePath{};
            std::vector<math::vec2> uv{math::vec2{0.f, 0.f}, math::vec2{1.f, 0.f},
                                       math::vec2{1.f, 1.f}, math::vec2{0.f, 1.f}};
            std::vector<common::uint8> data{};
            TextureStatus status{TextureStatus::INVALID};
        };

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

        struct ZLevel {
            oni::common::real32 majorLevelDelta{0.f};
            oni::common::real32 minorLevelDelta{0.f};

            oni::common::real32 level_0{0.f};
            oni::common::real32 level_1{0.f};
            oni::common::real32 level_2{0.f};
            oni::common::real32 level_3{0.f};
            oni::common::real32 level_4{0.f};
            oni::common::real32 level_5{0.f};
            oni::common::real32 level_6{0.f};
            oni::common::real32 level_7{0.f};
            oni::common::real32 level_8{0.f};
            oni::common::real32 level_9{0.f};
        };

    }
}