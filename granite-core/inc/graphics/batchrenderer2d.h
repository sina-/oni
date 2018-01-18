#pragma once


#include "renderer2d.h"
#include "renderable2d.h"
#include "buffers/indexbuffer.h"


namespace granite {
    namespace graphics {
        class BatchRenderer2D : public Renderer2D {

            static const unsigned long MAX_SPRITES = 10000;
            static const unsigned long MAX_VERTEX_SIZE = sizeof(VertexData);
            // Each sprite has 4 vertices.
            static const unsigned long MAX_SPRITE_SIZE = MAX_VERTEX_SIZE * 4;
            static const unsigned long MAX_BUFFER_SIZE = MAX_SPRITE_SIZE * MAX_SPRITES;
            // Each sprite has 6 indices.
            static const unsigned long MAX_INDICES_SIZE = MAX_SPRITE_SIZE * 6;

            std::unique_ptr<IndexBuffer> m_IBO;
            // Actual number of indices used.

            GLsizei m_IndexCount;

            // TODO: refactor VertexArray to use it here, the problem with VertexArray is that it can
            // only hold 1 sprite.
            GLuint m_VAO;
            // TODO: better name this as its not only holding vertex data but all the data.
            GLuint m_VBO;

        public:
            BatchRenderer2D();

            ~BatchRenderer2D() { glDeleteBuffers(1, &m_VBO); };

            void submit(const std::shared_ptr<Renderable2D> renderable) override;

            void flush() override;

        };

    }
}