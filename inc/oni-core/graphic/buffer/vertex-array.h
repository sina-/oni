#pragma once

#include <memory>

#include <oni-core/common/typedefs-graphics.h>

namespace oni {
    namespace buffer {

        class Buffer;

        class VertexArray {
        public:
            VertexArray(
                    const std::vector<graphic::BufferStructure> &,
                    common::oniGLsizei maxBufferSize);

            ~VertexArray();

            void
            bindVAO() const;

            void
            unbindVAO() const;

            void
            bindVBO() const;

            void
            unbindVBO() const;

        private:
            common::oniGLuint mArrayID{0};
            std::unique_ptr<Buffer> mVertexBuffers;
            std::vector<graphic::BufferStructure> mBufferStructure;
            common::oniGLsizei mMaxBufferSize{0};
        };

    }
}