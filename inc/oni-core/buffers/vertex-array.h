#pragma once

#include <memory>

#include <oni-core/common/typedefs-graphics.h>

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
            common::oniGLuint mArrayID;
            std::unique_ptr<Buffer> mVertexBuffers;
        };

    }
}