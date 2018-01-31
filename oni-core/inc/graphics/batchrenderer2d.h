#pragma once

#include <buffers/indexbuffer.h>
#include <graphics/renderable2d.h>
#include <graphics/renderer2d.h>
#include <graphics/utils/indexbuffergen.h>
#include <graphics/utils/checkoglerrors.h>
#include <buffers/buffer.h>
#include <buffers/vertexarray.h>


namespace oni {
    namespace graphics {
        using namespace buffers;

        class BatchRenderer2D : public Renderer2D {

            unsigned long m_MaxSpriteCount;
            unsigned long m_MaxIndicesCount;

            unsigned long m_MaxSpriteSize;
            unsigned long m_MaxBufferSize;
            GLsizei m_MaxVertexSize;

            std::unique_ptr<IndexBuffer> m_IBO;

            // Actual number of indices used.
            GLsizei m_IndexCount;

            GLuint m_VDO;
            std::unique_ptr<VertexArray> m_VAO;

            // The buffer that will hold all the VertexData in the batch.
            VertexData *m_Buffer;

        public:
            BatchRenderer2D(const unsigned long maxSpriteCount);

            ~BatchRenderer2D() { glDeleteBuffers(1, &m_VDO); };

            void begin() override;

            void submit(const std::unique_ptr<Renderable2D> &renderable) override;

            void flush() override;

            void end() override;

        };

    }
}