#pragma once

#include <vector>
#include <memory>
#include <utility>

#include <GL/glew.h>


namespace oni {
    namespace buffers {

        class Buffer;

        class VertexArray {
            GLuint mArrayID;
            std::unique_ptr<const Buffer> mVertexBuffers;

        public:
            explicit VertexArray(std::unique_ptr<const Buffer> vertexBuffer);

            ~VertexArray();

            void bindVAO() const;

            void unbindVAO() const;

            void bindVBO() const;

            void unbindVBO() const;

        };

    }
}