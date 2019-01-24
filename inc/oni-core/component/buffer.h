#pragma once

#include <memory>
#include <vector>

#include <oni-core/common/typedefs-graphics.h>
#include <oni-core/math/vec2.h>
#include <oni-core/math/vec3.h>
#include <oni-core/math/vec4.h>
#include <oni-core/math/mat4.h>

namespace oni {
    namespace component {
        struct Vertex {
            math::vec3 position{0.0f, 0.0f, 0.0f};
        };

        struct ColoredVertex {
            math::vec3 position{0.0f, 0.0f, 0.0f};
            math::vec4 color{0.0f, 0.0f, 0.0f, 0.0f};
        };

        struct TexturedVertex {
            math::vec3 position{0.0f, 0.0f, 0.0f};
            common::oniGLint samplerID{0};
            // TODO: use UNSIGNED_SHORT
            math::vec2 uv{0.0f, 0.0f};
        };

        struct BufferStructure {
            common::oniGLuint index{0};
            common::oniGLint componentCount{0};
            common::oniGLenum componentType{0};
            common::oniGLboolean normalized{0};
            common::oniGLsizei stride{0};
            const common::oniGLvoid *offset{nullptr};
        };

        typedef std::vector<std::unique_ptr<component::BufferStructure>> BufferStructureList;
    }
}
