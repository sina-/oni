#pragma once

#include <vector>
#include <memory>
#include <utility>

#include <GL/glew.h>

namespace oni {
    namespace buffers {

        class Buffer;

        class VertexArray {
        public:
            explicit VertexArray(std::unique_ptr<Buffer> vertexBuffer);

            ~VertexArray();

            void bindVAO() const;

            void unbindVAO() const;

            void bindVBO() const;

            void unbindVBO() const;

        private:
            GLuint mArrayID;
            std::unique_ptr<Buffer> mVertexBuffers;
        };

    }
}