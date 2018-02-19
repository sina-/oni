#pragma once

#include <math/vec2.h>
#include <math/vec3.h>
#include <math/vec4.h>
#include <math/mat4.h>

#include <GL/glew.h>
#include <memory>

namespace oni {
    namespace components {

        struct VertexData {
            oni::math::vec3 vertex;
            oni::math::vec4 color;
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

        typedef std::vector<std::unique_ptr<const BufferStructure>> BufferStructures;

        struct Renderable2D {
            math::vec4 color;
            /**
             *    B    C
             *    +----+
             *    |    |
             *    +----+
             *    A    D
             */
            math::vec3 positionA;
            math::vec3 positionB;
            math::vec3 positionC;
            math::vec3 positionD;

            Renderable2D() : color(math::vec4()), positionA(math::vec3()), positionB(math::vec3()),
                             positionC(math::vec3()), positionD(math::vec3()) {}

            Renderable2D(const math::vec4 &m_Color, const math::vec3 &m_PositionA, const math::vec3 &m_PositionB,
                         const math::vec3 &m_PositionC, const math::vec3 &m_PositionD) : color(m_Color),
                                                                                         positionA(m_PositionA),
                                                                                         positionB(m_PositionB),
                                                                                         positionC(m_PositionC),
                                                                                         positionD(m_PositionD) {}

            Renderable2D(const Renderable2D &other) = default;


/*            void setVertexPositions(const math::vec2 &size, const math::vec3 &pos) {
                m_PositionA = pos;
                positionB = math::vec3(pos.x, pos.y + size.y, pos.z);
                positionC = math::vec3(pos.x + size.x, pos.y + size.y, pos.z);
                positionD = math::vec3(pos.x + size.x, pos.y, pos.z);
            }*/
        };

        typedef std::vector<std::unique_ptr<Renderable2D>> Renderables;

    }
}