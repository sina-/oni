#pragma once

#include <memory>

#include <oni-core/common/typedefs-graphics.h>

namespace oni {
    namespace buffer {

        class Buffer;

        class VertexArray {
        public:
            explicit VertexArray(std::unique_ptr<Buffer> vertexBuffer, const std::vector<component::BufferStructure> &);

            ~VertexArray();

            void bindVAO() const;

            void unbindVAO() const;

            void bindVBO() const;

            void unbindVBO() const;

        private:
            common::oniGLuint mArrayID;
            std::unique_ptr<Buffer> mVertexBuffers;
            std::vector<component::BufferStructure> mBufferStructure;
        };

    }
}