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
        // TODO: These are not components and shouldn't be here, move them to graphics package
        struct Vertex {
            math::vec3 position{0.f, 0.f, 0.f};
        };

        struct ColoredVertex {
            math::vec3 position{0.f, 0.f, 0.f};
            math::vec4 color{0.f, 0.f, 0.f, 0.f};
        };

        struct TexturedVertex {
            math::vec3 position{0.f, 0.f, 0.f};
            common::oniGLint samplerID{0};
            // TODO: use UNSIGNED_SHORT
            math::vec2 uv{0.f, 0.f};
        };

        struct ParticleVertex {
            math::vec3 position{0.f, 0.f, 0.f};
            math::vec4 color{0.f, 0.f, 0.f, 0.f};
            common::real32 age{0.f};
            common::real32 heading{0.f};
            common::real32 velocity{0.f};
            common::oniGLint samplerID{-1};
            common::real32 halfSize{2.f};
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
