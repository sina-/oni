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
            math::vec3 position;

            Vertex() : position(math::vec3()) {}
        };

        struct ColoredVertex {
            math::vec3 position;
            math::vec4 color;

            ColoredVertex() : position(math::vec3()), color(math::vec4()) {}
        };

        struct TexturedVertex {
            math::vec3 position;
            GLint samplerID;
            // TODO: use UNSIGNED_SHORT
            oni::math::vec2 uv;

            TexturedVertex() : position(math::vec3()), samplerID(0), uv(math::vec2()) {}
        };

        struct BufferStructure {
            GLuint index;
            GLuint componentCount;
            GLenum componentType;
            GLboolean normalized;
            GLsizei stride;
            const GLvoid *offset;

            BufferStructure(GLuint index, GLuint componentCount, GLenum componentType, GLboolean normalized,
                            GLsizei stride, const void *offset) : index(index), componentCount(componentCount),
                                                                  componentType(componentType), normalized(normalized),
                                                                  stride(stride), offset(offset) {}
        };

    }
}
