#include <graphics/batch-renderer-2d.h>
#include <graphics/utils/index-buffer-gen.h>
#include <utils/oni-assert.h>
#include <components/physical.h>
#include <graphics/texture.h>

namespace oni {
    namespace graphics {
        BatchRenderer2D::BatchRenderer2D(const unsigned long maxSpriteCount) : m_IndexCount(0),
                                                                               m_MaxSpriteCount(maxSpriteCount) {
            // Each sprite has 6 indices.
            m_MaxIndicesCount = m_MaxSpriteCount * 6;
            m_MaxVertexSize = sizeof(components::VertexData);

            auto maxUIntSize = std::numeric_limits<unsigned int>::max();
            ONI_DEBUG_ASSERT(m_MaxIndicesCount < maxUIntSize);

            // Each sprite has 4 vertices (6 in reality but 4 of them share the same data).
            m_MaxSpriteSize = m_MaxVertexSize * 4;
            m_MaxBufferSize = m_MaxSpriteSize * m_MaxSpriteCount;

            // TODO: This has to match the shader layout. Can it be used to initialize shader layout?
            const int vertexBufferIndex = 0;
            const int colorBufferIndex = 1;
            const int textureIDBufferIndex = 2;
            const int textureCoordBufferIndex = 3;

            // TODO: I have assumed that every vertex uses every attribute in VertexData, however, if a Layer renders
            // un-textured vertices, for example, the memory is wasted. It makes sense for the Layer to be responsible for
            // creating the necessary BufferStructures that it requires. At that point I also have to figure out
            // how to use appropriate VertexBuffer structure that only has necessary attributes. One solution could be
            // to define, by inheritance, a new type of Layer that uses a new type of VertexBuffer created specially
            // to support that Layer. For example, TextureLayer could have TextureVertexBuffer. Plain TileLayer can
            // have ColorVertexBuffer.
            auto vertexBuffer = std::make_unique<const components::BufferStructure>
                    (vertexBufferIndex, 3, GL_FLOAT, GL_FALSE, m_MaxVertexSize, static_cast<const GLvoid *>(nullptr));
            auto colorBuffer = std::make_unique<const components::BufferStructure>
                    (colorBufferIndex, 4, GL_FLOAT, GL_TRUE, m_MaxVertexSize,
                     reinterpret_cast<const GLvoid *>(offsetof(components::VertexData, components::VertexData::color)));
            auto textureIDBuffer = std::make_unique<const components::BufferStructure>
                    (textureIDBufferIndex, 1, GL_FLOAT, GL_FALSE, m_MaxVertexSize,
                     reinterpret_cast<const GLvoid *>(offsetof(components::VertexData, components::VertexData::tid)));
            auto textureCoordBuffer = std::make_unique<const components::BufferStructure>
                    (textureCoordBufferIndex, 2, GL_FLOAT, GL_FALSE, m_MaxVertexSize,
                     reinterpret_cast<const GLvoid *>(offsetof(components::VertexData, components::VertexData::uv)));

            auto bufferStructures = components::BufferStructures();
            bufferStructures.push_back(std::move(vertexBuffer));
            bufferStructures.push_back(std::move(colorBuffer));
            bufferStructures.push_back(std::move(textureIDBuffer));
            bufferStructures.push_back(std::move(textureCoordBuffer));

            auto vbo = std::make_unique<buffers::Buffer>(std::vector<GLfloat>(), m_MaxBufferSize, GL_STATIC_DRAW,
                                                         std::move(bufferStructures));
            m_VAO = std::make_unique<buffers::VertexArray>(std::move(vbo));

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

            m_Samplers.assign(32, 0);
            std::iota(m_Samplers.begin(), m_Samplers.end(), 0);
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
            m_Buffer = (components::VertexData *) (glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
        }

        void BatchRenderer2D::submit(const components::Placement &position, const components::Appearance &appearance) {
            // Check if Buffer can handle the number of vertices.
            // TODO: This seems to trigger even in none-debug mode
            ONI_DEBUG_ASSERT(m_IndexCount + 6 < m_MaxIndicesCount);

            // First vertex.

            /** The vertices are absolute coordinates, there is no model matrix.
             *    b    c
             *    +----+
             *    |    |
             *    +----+
             *    a    d
             */
            // a.
            m_Buffer->vertex = position.vertexA;
            m_Buffer->color = appearance.color;
            m_Buffer++;

            // b.
            m_Buffer->vertex = position.vertexB;
            m_Buffer->color = appearance.color;
            m_Buffer++;

            // c.
            m_Buffer->vertex = position.vertexC;
            m_Buffer->color = appearance.color;
            m_Buffer++;

            // d.
            m_Buffer->vertex = position.vertexD;
            m_Buffer->color = appearance.color;
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

        void BatchRenderer2D::submit(const components::Placement &position, const components::Appearance &appearance,
                                     const components::Texture &texture) {
            // Check if Buffer can handle the number of vertices.
            ONI_DEBUG_ASSERT(m_IndexCount + 6 < m_MaxIndicesCount);

            auto textureID = texture.textureID;
            GLuint samplerID = 0;

            auto it = m_TextureToSampler.find(textureID);
            if (it == m_TextureToSampler.end()) {
                if (m_TextureToSampler.size() >= m_MaxTextureIDSupport) {
                    reset();
                }

                samplerID = m_Samplers.back();
                m_TextureToSampler[textureID] = samplerID;
                m_Samplers.pop_back();
            } else {
                samplerID = (*it).second;
            }

            /*
            GLuint tid = 0;

            auto it = std::find(m_SamplerTextureIDs.begin(), m_SamplerTextureIDs.end(), textureID);
            if (it != m_SamplerTextureIDs.end()) {
                tid = static_cast<GLuint>(std::distance(m_SamplerTextureIDs.begin(), it));
            } else {
                if (m_SamplerTextureIDs.size() >= m_MaxTextureIDSupport) {
                    // This means we have reached maximum number of texture IDs OpenGL can support
                    end();
                    flush();
                    begin();
                    m_SamplerTextureIDs.clear();
                }
                m_SamplerTextureIDs.push_back(textureID);

                // Texture id starts from 0. size() starts from 1.
                tid = static_cast<GLuint>(m_SamplerTextureIDs.size() - 1);

            }
             */

            m_Buffer->vertex = position.vertexA;
            m_Buffer->uv = texture.uv[0];
            m_Buffer->tid = samplerID;
            m_Buffer++;

            m_Buffer->vertex = position.vertexB;
            m_Buffer->uv = texture.uv[1];
            m_Buffer->tid = samplerID;
            m_Buffer++;

            m_Buffer->vertex = position.vertexC;
            m_Buffer->uv = texture.uv[2];
            m_Buffer->tid = samplerID;
            m_Buffer++;

            m_Buffer->vertex = position.vertexD;
            m_Buffer->uv = texture.uv[3];
            m_Buffer->tid = samplerID;
            m_Buffer++;

            m_IndexCount += 6;

        }

        void BatchRenderer2D::flush() {
/*            int index = 0;
            for (auto tid: m_SamplerTextureIDs) {
                // This means in the shader "uniform sampler2D textureIDs[index]"
                // corresponds to tid. For example if textureIDs[0] = 5, then
                // to assign a vertex textureID 5 you need to select sampler 0.
                glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + index));
                LoadTexture::bind(tid);

                index++;
            }*/
            for (const auto & tid2s: m_TextureToSampler) {
                glActiveTexture(GL_TEXTURE0 + tid2s.second);
                LoadTexture::bind(tid2s.first);
            }

            m_VAO->bindVAO();
            m_IBO->bind();

            glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, nullptr);

            m_IBO->unbind();
            m_VAO->unbindVAO();

            LoadTexture::unbind();

            m_IndexCount = 0;

        }

        void BatchRenderer2D::end() {
            glUnmapBuffer(GL_ARRAY_BUFFER);
            m_VAO->unbindVBO();

        }

    }
}
