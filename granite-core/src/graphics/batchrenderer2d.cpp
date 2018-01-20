#include <graphics/batchrenderer2d.h>
#include <graphics/utils/indexbuffergen.h>
#include <graphics/utils/checkoglerrors.h>

namespace granite {
    namespace graphics {
        // TODO: This function needs to reuse what buffers packages has to offer
        // instead of reimplementing.
        BatchRenderer2D::BatchRenderer2D() : m_IndexCount(0) {
            glGenVertexArrays(1, &m_VAO);
            glGenBuffers(1, &m_VBO);

            glBindVertexArray(m_VAO);
            glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

            glBufferData(GL_ARRAY_BUFFER, MAX_BUFFER_SIZE, nullptr, GL_DYNAMIC_DRAW);
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            // Vertex data as in Renderable2D::VertexData.vertex
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, MAX_VERTEX_SIZE, static_cast<const GLvoid *>(nullptr));
            // Color data as in Renderable2D::VertexData.color
            // First three elements are vertex data, then comes colors (as specified by offset)
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, MAX_VERTEX_SIZE,
                                  reinterpret_cast<const GLvoid *>(3 * sizeof(GLfloat)));

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            /***
             * This for loop is equivalent to IndexBufferGen and easier to understand but the later is
             * more fun and educational.
            GLushort offset = 0;
            for (auto i = 0; i < MAX_INDICES_SIZE; i += 6) {
                indices[i + 0] = offset + 0;
                indices[i + 1] = offset + 1;
                indices[i + 2] = offset + 2;

                indices[i + 4] = offset + 2;
                indices[i + 5] = offset + 3;
                indices[i + 6] = offset + 0;

                offset += 4;
            }
             ***/
            std::vector<GLushort> indices(MAX_INDICES_COUNT);
            IndexBufferGen gen;
            std::generate(indices.begin(), indices.end(), gen);

            m_IBO = std::make_unique<IndexBuffer>(indices, MAX_INDICES_COUNT);
            glBindVertexArray(0);

            CHECK_OGL_ERRORS

        }

        void BatchRenderer2D::begin() {
            glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
            /***
             * If you want to use smart pointers, you need to supply custom deleter:
             * struct custom_deleter
                  {
                    void operator ()( void const* ) const
                    {
                      glUnmapBuffer(GL_ARRAY_BUFFER);
                    }
                };
             * to the raw pointer, and do something like:
             * m_Buffer = std::unique_ptr<VertexData, custom_deleter>(
                reinterpret_cast<VertexData*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)),
                    custom_deleter(GL_ARRAY_BUFFER) );
             * For more details: https://github.com/sina-/ehgl/blob/master/eg/buffer_target.hpp#L159
             ***/
            m_Buffer = (VertexData *) (glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
        }

        // TODO make sure renderable is not copied in each invocation.
        void BatchRenderer2D::submit(const std::shared_ptr<const Renderable2D> renderable) {
            if (m_IndexCount + 6 >= MAX_INDICES_COUNT) {
                throw std::runtime_error("Too many objects to render!");
            }

            // First vertex.
            auto position = renderable->getPosition();
            auto size = renderable->getSize();
            auto color = renderable->getColor();

            // TODO make sure such assignments increase the reference count to renderable
            // otherwise if renderable is de-allocated m_Buffer will point to garbage.

            /** The vertices are absolute coordinates, there is no model matrix.
             *    b    c
             *    +----+
             *    |    |
             *    +----+
             *    a    d
             */
            // a.
            m_Buffer->vertex = position;
            m_Buffer->color = color;
            m_Buffer++;

            // b.
            m_Buffer->vertex = math::vec3(position.x, position.y + size.y, position.z);
            m_Buffer->color = color;
            m_Buffer++;

            // c.
            m_Buffer->vertex = math::vec3(position.x + size.x, position.y + size.y, position.z);
            m_Buffer->color = color;
            m_Buffer++;

            // d.
            m_Buffer->vertex = math::vec3(position.x + size.x, position.y, position.z);
            m_Buffer->color = color;
            m_Buffer++;


            // +6 as there are 6 vertices that makes up two adjacent triangles but those triangles are
            // defined by 4 vertices only.
            /**
             *    1 +---+ 0
             *      |  /|
             *      |/  |
             *    2 +---+ 3
             **/
            m_IndexCount += 6;
        }

        void BatchRenderer2D::flush() {
            glBindVertexArray(m_VAO);
            m_IBO->bind();

            glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_SHORT, 0);

            m_IBO->unbind();
            glBindVertexArray(0);

            m_IndexCount = 0;
        }

        void BatchRenderer2D::end() {
            glUnmapBuffer(GL_ARRAY_BUFFER);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

        }

    }
}
