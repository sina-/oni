#pragma once

#include <memory>
#include <vector>

#include <GL/glew.h>

#include <oni-core/math/vec2.h>
#include <oni-core/math/vec3.h>
#include <oni-core/math/vec4.h>

namespace oni {
    namespace components {
        struct Vertex {
            math::vec3 position{0.0f, 0.0f, 0.0f};
        };

        struct ColoredVertex {
            math::vec3 position{0.0f, 0.0f, 0.0f};
            math::vec4 color{0.0f, 0.0f, 0.0f, 0.0f};
        };

        struct TexturedVertex {
            math::vec3 position{0.0f, 0.0f, 0.0f};
            GLint samplerID{0};
            // TODO: use UNSIGNED_SHORT
            math::vec2 uv{0.0f, 0.0f};
        };

        struct BufferStructure {
            GLuint index{0};
            GLuint componentCount{0};
            GLenum componentType{0};
            GLboolean normalized{0};
            GLsizei stride{0};
            const GLvoid *offset{nullptr};
        };

    }
}
