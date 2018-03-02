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
            explicit VertexArray(std::unique_ptr<const Buffer> vertexBuffer);

            ~VertexArray() { glDeleteVertexArrays(1, &m_ArrayID); };

            void bindVAO() const { glBindVertexArray(m_ArrayID); }

            void unbindVAO() const { glBindVertexArray(0); }

            void bindVBO() const { m_VertexBuffers->bind(); }

            void unbindVBO() const { m_VertexBuffers->unbind(); }

        };

    }
}