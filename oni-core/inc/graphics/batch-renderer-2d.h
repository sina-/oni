#pragma once

#include <buffers/index-buffer.h>
#include <graphics/renderer-2d.h>
#include <graphics/utils/check-ogl-errors.h>
#include <buffers/vertex-array.h>
#include <components/visual.h>
#include <components/physical.h>
#include <map>
#include <stack>
#include <utils/oni-assert.h>

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

            // The buffer that will hold components::Vertex data in the batch.
            void *m_Buffer;

            std::map<GLuint, GLuint> m_TextureToSampler;
            std::vector<GLuint> m_Samplers;

            const unsigned long m_MaxNumTextureSamplers;

        public:
            BatchRenderer2D(const unsigned long maxSpriteCount, unsigned long maxNumTextureSamplers,
                            GLsizei maxVertexSize,
                            components::BufferStructures bufferStructures);

            ~BatchRenderer2D() { glDeleteBuffers(1, &m_VDO); };

            void begin() override;

            void submit(const components::Placement &position, const components::Appearance &color) override;

            void submit(const components::Placement &position, const components::Appearance &color,
                        const components::Texture &texture) override;

            void flush() override;

            void end() override;

        private:
            void reset();
        };

    }
}