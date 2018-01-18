#include <graphics/batchrenderer2d.h>
#include <graphics/utils/indexbuffergen.h>

namespace granite {
    namespace graphics {
        // TODO: This function needs to reuse what buffers packages has to offer
        // instead of reimplementing.
        BatchRenderer2D::BatchRenderer2D() {
            glGenBuffers(1, &m_VAO);
            glGenBuffers(1, &m_VBO);

            glBindVertexArray(m_VAO);
            glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

            glBufferData(GL_ARRAY_BUFFER, MAX_BUFFER_SIZE, nullptr, GL_DYNAMIC_DRAW);
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            // Vertex data as in Renderable2D::VertexData.vertex
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, MAX_VERTEX_SIZE, nullptr);
            // Color data as in Renderable2D::VertexData.color
            // First three elements are vertex data, then comes colors (as specified by offset)
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, MAX_VERTEX_SIZE,
                                  reinterpret_cast<const GLvoid *>(3 * GL_FLOAT));

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
            std::vector<GLushort> indices(MAX_INDICES_SIZE);
            IndexBufferGen gen;
            std::generate(indices.begin(), indices.end(), gen);

            m_IBO = std::make_unique<IndexBuffer>(indices, MAX_INDICES_SIZE);
            glBindVertexArray(0);
        }

        void BatchRenderer2D::submit(const std::shared_ptr<Renderable2D> renderable) {

        }

        void BatchRenderer2D::flush() {


        }

    }
}
