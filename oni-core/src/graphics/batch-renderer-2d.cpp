#include <graphics/batch-renderer-2d.h>
#include <graphics/utils/index-buffer-gen.h>
#include <graphics/texture.h>
#include <utils/io.h>

namespace oni {
    namespace graphics {
        BatchRenderer2D::BatchRenderer2D(const unsigned long maxSpriteCount, unsigned long maxNumTextureSamplers,
                                         GLsizei maxVertexSize,
                                         components::BufferStructures bufferStructures)
                : m_IndexCount(0),
                  m_MaxSpriteCount(maxSpriteCount),
                  m_MaxVertexSize(maxVertexSize),
                  m_MaxNumTextureSamplers(maxNumTextureSamplers) {

            // Each sprite has 6 indices.
            m_MaxIndicesCount = m_MaxSpriteCount * 6;

            auto maxUIntSize = std::numeric_limits<unsigned int>::max();
            ONI_DEBUG_ASSERT(m_MaxIndicesCount < maxUIntSize);

            // Each sprite has 4 vertices (6 in reality but 4 of them share the same data).
            m_MaxSpriteSize = m_MaxVertexSize * 4;
            m_MaxBufferSize = m_MaxSpriteSize * m_MaxSpriteCount;

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

            m_Samplers = generateSamplerIDs();

        }

        void BatchRenderer2D::begin() {
            m_VAO->bindVBO();
            // Data written to m_Buffer has to match the structure of VBO.
            m_Buffer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        }

        void BatchRenderer2D::submit(const components::Placement &position, const components::Appearance &appearance) {
            // Check if Buffer can handle the number of vertices.
            // TODO: This seems to trigger even in none-debug mode
            ONI_DEBUG_ASSERT(m_IndexCount + 6 < m_MaxIndicesCount);

            auto buffer = static_cast<components::ColoredVertex *>(m_Buffer);

            /** The vertices are absolute coordinates, there is no model matrix.
             *    b    c
             *    +----+
             *    |    |
             *    +----+
             *    a    d
             */

            // a.
            buffer->position = position.vertexA;
            buffer->color = appearance.color;
            buffer++;

            // b.
            buffer->position = position.vertexB;
            buffer->color = appearance.color;
            buffer++;

            // c.
            buffer->position = position.vertexC;
            buffer->color = appearance.color;
            buffer++;

            // d.
            buffer->position = position.vertexD;
            buffer->color = appearance.color;
            buffer++;

            // Update the m_Buffer to point to the head.
            m_Buffer = static_cast<void *>(buffer);

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

        void BatchRenderer2D::submit(const components::Placement &position, const components::Texture &texture) {
            // Check if Buffer can handle the number of vertices.
            ONI_DEBUG_ASSERT(m_IndexCount + 6 < m_MaxIndicesCount);

            auto samplerID = getSamplerID(texture.textureID);

            auto buffer = static_cast<components::TexturedVertex *>(m_Buffer);

            buffer->position = position.vertexA;
            buffer->uv = texture.uv[0];
            buffer->samplerID = samplerID;
            buffer++;

            buffer->position = position.vertexB;
            buffer->uv = texture.uv[1];
            buffer->samplerID = samplerID;
            buffer++;

            buffer->position = position.vertexC;
            buffer->uv = texture.uv[2];
            buffer->samplerID = samplerID;
            buffer++;

            buffer->position = position.vertexD;
            buffer->uv = texture.uv[3];
            buffer->samplerID = samplerID;
            buffer++;

            m_Buffer = static_cast<void *>(buffer);

            m_IndexCount += 6;

        }

        // TODO: submit needs not to know about fontmanager. Find the glyph and load the relevant data into
        // Text component during init, and just read them during submit.
        void BatchRenderer2D::submit(const components::Text &text, const graphics::FontManager &fontManager) {
            auto buffer = static_cast<components::TexturedVertex *>(m_Buffer);

            auto samplerID = getSamplerID(fontManager.getTextureID());

            auto advance = 0.0f;

            float scaleX = 1600.0f / 64.0f;
            float scaleY = 900.0f / 38.0f;

            for (auto character: text.text) {
                ONI_DEBUG_ASSERT(m_IndexCount + 6 < m_MaxIndicesCount);

                auto glyph = fontManager.findGlyph(character);

                auto x0 = text.position.x + glyph->offset_x / scaleX + advance;
                auto y0 = text.position.y + glyph->offset_y / scaleY;
                auto x1 = x0 + glyph->width / scaleX;
                auto y1 = y0 - glyph->height / scaleY;

                auto u0 = glyph->s0;
                auto v0 = glyph->t0;
                auto u1 = glyph->s1;
                auto v1 = glyph->t1;

                buffer->position = math::vec3(x0, y0, 1);
                buffer->uv = math::vec2(u0, v0);
                buffer->samplerID = samplerID;
                buffer++;

                buffer->position = math::vec3(x0, y1, 1);
                buffer->uv = math::vec2(u0, v1);
                buffer->samplerID = samplerID;
                buffer++;

                buffer->position = math::vec3(x1, y1, 1);
                buffer->uv = math::vec2(u1, v1);
                buffer->samplerID = samplerID;
                buffer++;

                buffer->position = math::vec3(x1, y0, 1);
                buffer->uv = math::vec2(u1, v0);
                buffer->samplerID = samplerID;
                buffer++;

                advance += glyph->advance_x / scaleX;
                m_IndexCount += 6;
            }

            m_Buffer = static_cast<void *>(buffer);

        }

        void BatchRenderer2D::flush() {
            for (const auto &t2s: m_TextureToSampler) {
                glActiveTexture(GL_TEXTURE0 + t2s.second);
                LoadTexture::bind(t2s.first);
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

        void BatchRenderer2D::reset() {
            end();
            flush();
            begin();
            m_Samplers.assign(m_MaxNumTextureSamplers, 0);
            // Fill the vector with 0, 1, 2, 3, ...
            std::iota(m_Samplers.begin(), m_Samplers.end(), 0);
            m_TextureToSampler.clear();
        }

        GLint BatchRenderer2D::getSamplerID(GLuint textureID) {
            auto it = m_TextureToSampler.find(textureID);
            GLint samplerID = 0;

            if (it == m_TextureToSampler.end()) {
                ONI_DEBUG_ASSERT(m_TextureToSampler.size() < m_MaxNumTextureSamplers);

                /*
                 * To support more than m_MaxNumTextureSamplers following can be used. But,
                 * this operation is very expensive. Instead create more layers if needed.
                if (m_TextureToSampler.size() >= m_MaxNumTextureSamplers) {
                    reset();
                }
                */

                samplerID = m_Samplers.back();
                m_TextureToSampler[textureID] = samplerID;
                m_Samplers.pop_back();
            } else {
                samplerID = (*it).second;
            }

            return samplerID;
        }

        std::vector<GLint> BatchRenderer2D::generateSamplerIDs() {
            std::vector<GLint> samplers;
            samplers.assign(m_MaxNumTextureSamplers, 0);
            // Fill the vector with 0, 1, 2, 3, ...
            std::iota(samplers.begin(), samplers.end(), 0);
            return samplers;
        }
    }
}
