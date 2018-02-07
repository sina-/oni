#pragma once

#include <memory>
#include <GL/glew.h>
#include <math/vec2.h>
#include <math/vec3.h>
#include <math/vec4.h>
#include <math/mat4.h>
#include <graphics/shader.h>

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
            const GLvoid *offset;
        };

        typedef std::vector<std::unique_ptr<const BufferStructure>> BufferStructures;

        // TODO: make this an interface and sprite what this class is now.
        // TODO: Unusual to have m_Position and m_PositionX at the same time, settle for one.
        class Renderable2D {
        protected:
            math::vec2 m_Size;
            math::vec3 m_Position;
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

        public:
            Renderable2D(const math::vec2 &size, const math::vec3 &pos, const math::vec4 &color)
                    : m_Size(size), m_Position(pos), m_Color(color) {
                setVertexPositions(size, pos);
            }


            virtual ~Renderable2D() = default;

            // TODO: Re-think the use the definition of this function. Do you really need Renderable2D
            // to define this?
            virtual void update(int key, const std::unique_ptr<Shader> &shader) = 0;

            Renderable2D &operator=(const Renderable2D &) = delete;

            Renderable2D &operator=(Renderable2D &) = delete;

            inline const math::vec2 &getSize() const { return m_Size; }

            inline const math::vec3 &getPosition() const { return m_Position; }

            inline const math::vec4 &getColor() const { return m_Color; }

            inline const math::vec3 &getPositionA() const { return m_PositionA; }

            inline const math::vec3 &getPositionB() const { return m_PositionB; }

            inline const math::vec3 &getPositionC() const { return m_PositionC; }

            inline const math::vec3 &getPositionD() const { return m_PositionD; }

            void setVertexPositions(const math::vec2 &size, const math::vec3 &pos) {
                m_PositionA = pos;
                m_PositionB = math::vec3(pos.x, pos.y + size.y, pos.z);
                m_PositionC = math::vec3(pos.x + size.x, pos.y + size.y, pos.z);
                m_PositionD = math::vec3(pos.x + size.x, pos.y, pos.z);
            }
        };

        typedef std::vector<const std::unique_ptr<Renderable2D> &> RenderableReferences;
        typedef std::vector<std::unique_ptr<Renderable2D>> Renderables;

        class StaticSprite : public Renderable2D {
        public:
            StaticSprite(const math::vec2 &size, const math::vec3 &pos, const math::vec4 &color) : Renderable2D(
                    size, pos, color) {}

            void update(int key, const std::unique_ptr<Shader> &shader) override {};

        };

        class DynamicRenderable2D : public Renderable2D {
        public:
            DynamicRenderable2D(const math::vec2 &size, const math::vec3 &pos, const math::vec4 &color) : Renderable2D(
                    size, pos, color) {}

            virtual void update(int key, const std::unique_ptr<Shader> &shader) override = 0;
        };
    }
}