#pragma once

#include <memory>
#include <GL/glew.h>
#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"
#include "math/mat4.h"

namespace oni {
    namespace graphics {

        // TODO: move this out
        struct VertexData {
            math::vec3 vertex;
            math::vec4 color;
        };

        // TODO: move this out
        struct BufferStructure {
            BufferStructure(GLuint index, GLuint componentCount, GLenum componentType, GLboolean normalized,
                            GLsizei stride, const void *offset) : index(index), componentCount(componentCount),
                                                                  componentType(componentType), normalized(normalized),
                                                                  stride(stride), offset(offset) {}
            GLuint index;
            GLuint componentCount;
            GLenum componentType;
            GLboolean normalized;
            GLsizei stride;
            const GLvoid * offset;
        };
        typedef std::vector<std::unique_ptr<const BufferStructure>> BufferStructures;

        class Renderable2D {
            math::vec2 m_Size;
            math::vec3 m_Position;
            math::vec4 m_Color;

            /**
             *    b    c
             *    +----+
             *    |    |
             *    +----+
             *    a    d
             */
            math::vec3 m_PositionA;
            math::vec3 m_PositionB;
            math::vec3 m_PositionC;
            math::vec3 m_PositionD;

        public:
            Renderable2D(const math::vec2 &size, const math::vec3 &pos, const math::vec4 &color)
                    : m_Size(size), m_Position(pos), m_Color(color) {

                m_PositionA = pos;
                m_PositionB = math::vec3(pos.x, pos.y + size.y, pos.z);
                m_PositionC = math::vec3(pos.x + size.x, pos.y + size.y, pos.z);
                m_PositionD = math::vec3(pos.x + size.x, pos.y, pos.z);

/*                auto vertices = std::vector<GLfloat>{
                        0, 0, 0,
                        0, size.y, 0,
                        size.x, size.y, 0,
                        size.x, 0, 0
                };

                auto colors = std::vector<GLfloat>{
                        color.x, color.y, color.z, color.w,
                        color.x, color.y, color.z, color.w,
                        color.x, color.y, color.z, color.w,
                        color.x, color.y, color.z, color.w
                };*/
            }


            virtual ~Renderable2D() = default;

            Renderable2D & operator=(const Renderable2D&) = delete;
            Renderable2D & operator=(Renderable2D&) = delete;

            inline const math::vec2 &getSize() const { return m_Size; }

            inline const math::vec3 &getPosition() const { return m_Position; }

            inline const math::vec4 &getColor() const { return m_Color; }

            inline const math::vec3 &getPositionA() const { return m_PositionA; }

            inline const math::vec3 &getPositionB() const { return m_PositionB; }

            inline const math::vec3 &getPositionC() const { return m_PositionC; }

            inline const math::vec3 &getPositionD() const { return m_PositionD; }
        };
    }
}