#pragma once

#include <memory>

#include <oni-core/common/oni-common-typedefs-graphic.h>
#include <oni-core/graphic/buffer/oni-graphic-buffer-data.h>

namespace oni {
    class Buffer;

    class VertexArray {
    public:
        VertexArray(
                const std::vector<BufferStructure> &,
                oniGLsizei maxBufferSize);

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
        oniGLuint mArrayID{0};
        std::unique_ptr<Buffer> mVertexBuffers;
        std::vector<BufferStructure> mBufferStructure;
        oniGLsizei mMaxBufferSize{0};
    };
}