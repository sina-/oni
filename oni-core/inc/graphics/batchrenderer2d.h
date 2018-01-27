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

            // TODO: move them to constructor
            const unsigned long MAX_SPRITE_COUNT = 10000;
            // Each sprite has 6 indices.
            const unsigned long MAX_INDICES_COUNT = MAX_SPRITE_COUNT * 6;

            const GLsizei MAX_VERTEX_SIZE = sizeof(VertexData);
            // Each sprite has 4 vertices (6 in reality but 4 of them share the same data).
            const unsigned long MAX_SPRITE_SIZE = static_cast<const unsigned long>(MAX_VERTEX_SIZE * 4);
            const unsigned long MAX_BUFFER_SIZE = MAX_SPRITE_SIZE * MAX_SPRITE_COUNT;

            std::unique_ptr<IndexBuffer> m_IBO;

            // Actual number of indices used.
            GLsizei m_IndexCount;

            GLuint m_VDO;
            std::unique_ptr<VertexArray> m_VAO;

            // The buffer that will hold all the VertexData in the batch.
            VertexData *m_Buffer;

        public:
            BatchRenderer2D();

            ~BatchRenderer2D() { glDeleteBuffers(1, &m_VDO); };

            void begin() override;

            void submit(const std::shared_ptr<const Renderable2D> renderable) override;

            void flush() override;

            void end() override;

        };

    }
}