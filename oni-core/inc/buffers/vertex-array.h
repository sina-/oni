#pragma once

#include <GL/glew.h>
#include <vector>
#include <memory>
#include <utility>
#include "buffers/buffer.h"

namespace oni {
    namespace buffers {

        class VertexArray {
            GLuint mArrayID;
            std::unique_ptr<const Buffer> mVertexBuffers;

        public:
            explicit VertexArray(std::unique_ptr<const Buffer> vertexBuffer);

            ~VertexArray() { glDeleteVertexArrays(1, &mArrayID); };

            void bindVAO() const { glBindVertexArray(mArrayID); }

            void unbindVAO() const { glBindVertexArray(0); }

            void bindVBO() const { mVertexBuffers->bind(); }

            void unbindVBO() const { mVertexBuffers->unbind(); }

        };

    }
}