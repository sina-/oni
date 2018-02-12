#include <graphics/batchrenderer2d.h>
#include <graphics/utils/indexbuffergen.h>
#include <utils/oni_assert.h>

namespace oni {
    namespace graphics {
        BatchRenderer2D::BatchRenderer2D(const unsigned long maxSpriteCount) : m_IndexCount(0),
                                                                               m_MaxSpriteCount(maxSpriteCount) {
            // Each sprite has 6 indices.
            m_MaxIndicesCount = m_MaxSpriteCount * 6;
            m_MaxVertexSize = sizeof(VertexData);

            auto maxUIntSize = std::numeric_limits<unsigned int>::max();
            ONI_DEBUG_ASSERT(m_MaxIndicesCount < maxUIntSize)

            // Each sprite has 4 vertices (6 in reality but 4 of them share the same data).
            m_MaxSpriteSize = m_MaxVertexSize * 4;
            m_MaxBufferSize = m_MaxSpriteSize * m_MaxSpriteCount;

            auto vertexBuffer = std::make_unique<const BufferStructure>(
                    0, 3, GL_FLOAT, GL_FALSE, m_MaxVertexSize, static_cast<const GLvoid *>(nullptr));
            auto colorBuffer = std::make_unique<const BufferStructure>
                    (1, 4, GL_FLOAT, GL_FALSE, m_MaxVertexSize,
                     reinterpret_cast<const GLvoid *>(offsetof(VertexData, VertexData::color)));

            auto bufferStructures = BufferStructures();
            bufferStructures.push_back(std::move(vertexBuffer));
            bufferStructures.push_back(std::move(colorBuffer));

            auto vbo = std::make_unique<buffers::Buffer>(std::vector<GLfloat>(), m_MaxBufferSize, GL_STATIC_DRAW,
                                                         std::move(bufferStructures));
            m_VAO = std::make_unique<buffers::VertexArray>();
            m_VAO->addBuffer(std::move(vbo));

            std::vector<GLuint> indices(m_MaxIndicesCount);

            /**
             * This for loop is equivalent to IndexBufferGen and easier to understand but the later is
             * more fun and educational.
             */
            /*
            GLushort offset = 0;
            for (auto i = 0; i < m_MaxIndicesCount; i += 6) {
                indices[i + 0] = offset + 0;
                indices[i + 1] = offset + 1;
                indices[i + 2] = offset + 2;

                indices[i + 4] = offset + 2;
                indices[i + 5] = offset + 3;
                indices[i + 6] = offset + 0;

                offset += 4;
            }*/
            IndexBufferGen<GLuint> gen;
            std::generate(indices.begin(), indices.end(), gen);

            m_IBO = std::make_unique<buffers::IndexBuffer>(indices, m_MaxIndicesCount);

            // TODO: ~BatchRenderer2D wont be called if this check throws.
            CHECK_OGL_ERRORS

        }

        void BatchRenderer2D::begin() {
            m_VAO->bindVBO();
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
            CHECK_OGL_ERRORS
        }

        void BatchRenderer2D::submit(const Renderable2D &renderable) {
            if (m_IndexCount + 6 >= m_MaxIndicesCount) {
                throw std::runtime_error("Too many objects to render!");
            }

            // First vertex.
            auto color = renderable.getColor();

            /** The vertices are absolute coordinates, there is no model matrix.
             *    b    c
             *    +----+
             *    |    |
             *    +----+
             *    a    d
             */
            // a.
            m_Buffer->vertex = m_TransformationStack.back() * renderable.getPositionA();
            m_Buffer->color = color;
            m_Buffer++;

            // b.
            m_Buffer->vertex = m_TransformationStack.back() *renderable.getPositionB();
            m_Buffer->color = color;
            m_Buffer++;

            // c.
            m_Buffer->vertex = m_TransformationStack.back() * renderable.getPositionC();
            m_Buffer->color = color;
            m_Buffer++;

            // d.
            m_Buffer->vertex =m_TransformationStack.back() * renderable.getPositionD();
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
            m_VAO->bindVAO();
            m_IBO->bind();

            glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, nullptr);

            m_IBO->unbind();
            m_VAO->unbindVAO();

            m_IndexCount = 0;

            CHECK_OGL_ERRORS
        }

        void BatchRenderer2D::end() {
            glUnmapBuffer(GL_ARRAY_BUFFER);
            m_VAO->unbindVBO();

        }

    }
}
