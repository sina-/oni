#pragma once


#include "renderer2d.h"
#include "renderable2d.h"
#include "buffers/indexbuffer.h"


namespace granite {
    namespace graphics {
        class BatchRenderer2D : public Renderer2D {

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

            // TODO: refactor VertexArray to use it here, the problem with VertexArray is that it can
            // only hold 1 sprite.
            GLuint m_VAO;
            // TODO: better name this as its not only holding vertex data but all the data.
            GLuint m_VBO;

            // The buffer that will hold all the VertexData in the batch.
            VertexData *m_Buffer;

        public:
            BatchRenderer2D();

            ~BatchRenderer2D() { glDeleteBuffers(1, &m_VBO); };

            void begin();

            void submit(const std::shared_ptr<const Renderable2D> renderable) override;

            void flush() override;

            void end();

        };

    }
}