#pragma once

#include <GL/glew.h>
#include <vector>
#include <memory>
#include <utility>
#include "buffers/buffer.h"

namespace oni {
    namespace buffers {

        class VertexArray {
            GLuint m_ArrayID;
            std::unique_ptr<const Buffer> m_VertexBuffers;

        public:
            VertexArray();

            ~VertexArray() { glDeleteVertexArrays(1, &m_ArrayID); };

            void addBuffer(std::unique_ptr<const Buffer> vertexBuffer);

            inline void bindVAO() const { glBindVertexArray(m_ArrayID); }

            inline void unbindVAO() const { glBindVertexArray(0); }

            inline void bindVBO() const { m_VertexBuffers->bind(); }

            inline void unbindVBO() const { m_VertexBuffers->unbind(); }

        };

    }
}