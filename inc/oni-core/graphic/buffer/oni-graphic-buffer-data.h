#pragma once

#include <memory>
#include <vector>

#include <oni-core/common/oni-common-typedefs-graphic.h>
#include <oni-core/math/oni-math-vec2.h>
#include <oni-core/math/oni-math-vec3.h>
#include <oni-core/math/oni-math-vec4.h>
#include <oni-core/math/oni-math-mat4.h>

namespace oni {
    namespace graphic {
        struct TessellationVertex {
            common::oniGLint samplerID{-1};
            common::r32 heading{0.f};
            math::vec2 halfSize{1.f};
            math::vec3 position{0.f, 0.f, 0.f};
            math::vec4 color{0.f, 0.f, 0.f, 0.f};
            math::vec2 uv[4]{{0, 0},
                             {0, 1},
                             {1, 1},
                             {1, 0}};
        };

        struct StripVertex {
            math::vec4 center{0.f, 0.f, 0.f, 0.f};
            common::r32 texoff{0.f};
            math::vec3 bc{};
            //math::vec4 color{0.f, 0.f, 0.f, 0.f};
            //common::oniGLint samplerID{-1};
        };

        struct QuadVertex {
            common::oniGLint samplerFront{-1};
            common::oniGLint samplerBack{-1};
            math::vec4 color{0.f, 0.f, 0.f, 0.f};
            math::vec2 uv{};
            math::vec3 pos{};
        };

        struct BufferStructure {
            common::oniGLuint index{0};
            common::oniGLint componentCount{0};
            common::oniGLenum componentType{0};
            common::oniGLboolean normalized{0};
            common::oniGLsizei stride{0};
            const common::oniGLvoid *offset{nullptr};
        };
    }
}
