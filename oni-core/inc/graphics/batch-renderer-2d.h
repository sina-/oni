#pragma once

#include <map>
#include <stack>

#include <ftgl/freetype-gl.h>
#include <ftgl/texture-atlas.h>

#include <buffers/index-buffer.h>
#include <buffers/vertex-array.h>
#include <components/visual.h>
#include <components/physical.h>
#include <graphics/renderer-2d.h>
#include <utils/oni-assert.h>
#include <graphics/font-manager.h>

namespace oni {
    namespace graphics {
        using namespace buffers;

        class BatchRenderer2D : public Renderer2D {

            unsigned long m_MaxSpriteCount;
            unsigned long m_MaxIndicesCount;

            unsigned long m_MaxBufferSize;
            GLsizei m_MaxVertexSize;
            GLsizei m_MaxSpriteSize;

            std::unique_ptr<IndexBuffer> m_IBO;

            // Actual number of indices used.
            GLsizei m_IndexCount;

            GLuint m_VDO;
            std::unique_ptr<VertexArray> m_VAO;

            // The buffer that will hold components::Vertex data, or its variants, in the batch.
            void *m_Buffer;

            // A mapping from texture id to 2D sampler that will draw it.
            std::map<GLuint, GLint> m_TextureToSampler;
            std::vector<GLint> m_Samplers;

            const unsigned long m_MaxNumTextureSamplers;

        public:
            BatchRenderer2D(const unsigned long maxSpriteCount, unsigned long maxNumTextureSamplers,
                            GLsizei maxVertexSize,
                            components::BufferStructures bufferStructures);

            ~BatchRenderer2D() { glDeleteBuffers(1, &m_VDO); };

            void begin() override;

            void submit(const components::Placement &position, const components::Appearance &color) override;

            void submit(const components::Placement &position, const components::Texture &texture) override;

            void submit(const components::Text &text, const graphics::FontManager &fontManager) override;

            void flush() override;

            void end() override;

        private:
            void reset();

            // TODO: checkout texture arrays.
            /**
             * There are 0, 1, ..., m_MaxNumTextureSamplers texture samplers are available.
             * Each texture is assigned one and the id to the sampler is saved as part of vertex data
             * in the vertex buffer. During rendering in the shader the proper sampler is selected based
             * on the sampler id in the buffer.
             */
            GLint getSamplerID(GLuint textureID);

        public:
            std::vector<GLint> generateSamplerIDs();
        };

    }
}