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
            math::vec4 m_Color;
            /**
             *    B    C
             *    +----+
             *    |    |
             *    +----+
             *    A    D
             */
            math::vec3 m_PositionA;
            math::vec3 m_PositionB;
            math::vec3 m_PositionC;
            math::vec3 m_PositionD;

            Renderable2D(const math::vec4 &m_Color, const math::vec3 &m_PositionA, const math::vec3 &m_PositionB,
                         const math::vec3 &m_PositionC, const math::vec3 &m_PositionD) : m_Color(m_Color),
                                                                                         m_PositionA(m_PositionA),
                                                                                         m_PositionB(m_PositionB),
                                                                                         m_PositionC(m_PositionC),
                                                                                         m_PositionD(m_PositionD) {}

            Renderable2D &operator=(const Renderable2D &) = delete;

            Renderable2D &operator=(Renderable2D &) = delete;

/*            void setVertexPositions(const math::vec2 &size, const math::vec3 &pos) {
                m_PositionA = pos;
                m_PositionB = math::vec3(pos.x, pos.y + size.y, pos.z);
                m_PositionC = math::vec3(pos.x + size.x, pos.y + size.y, pos.z);
                m_PositionD = math::vec3(pos.x + size.x, pos.y, pos.z);
            }*/
        };

        typedef std::vector<std::unique_ptr<Renderable2D>> Renderables;

    }
}