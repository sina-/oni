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

        struct Placement {
            /**
             *    B    C
             *    +----+
             *    |    |
             *    +----+
             *    A    D
             */
            math::vec3 vertexA;
            math::vec3 vertexB;
            math::vec3 vertexC;
            math::vec3 vertexD;

            Placement() : vertexA(math::vec3()), vertexB(math::vec3()),
                       vertexC(math::vec3()), vertexD(math::vec3()) {}

            Placement(const math::vec3 &m_PositionA, const math::vec3 &m_PositionB,
                   const math::vec3 &m_PositionC, const math::vec3 &m_PositionD) :
                    vertexA(m_PositionA),
                    vertexB(m_PositionB),
                    vertexC(m_PositionC),
                    vertexD(m_PositionD) {}

            Placement(const Placement &other) = default;

/*            void setVertexPositions(const math::vec2 &size, const math::vec3 &pos) {
                m_PositionA = pos;
                vertexB = math::vec3(pos.x, pos.y + size.y, pos.z);
                vertexC = math::vec3(pos.x + size.x, pos.y + size.y, pos.z);
                vertexD = math::vec3(pos.x + size.x, pos.y, pos.z);
            }*/
        };

        struct Appearance {
            math::vec4 color;

            Appearance() : color(math::vec4()) {}

            explicit Appearance(const math::vec4 &col) : color(col) {}

            Appearance(const Appearance &other) {
                color.x = other.color.x;
                color.y = other.color.y;
                color.z = other.color.z;
                color.w = other.color.w;
            }
        };

    }
}