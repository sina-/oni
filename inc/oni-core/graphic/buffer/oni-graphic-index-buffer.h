#pragma once

#include <vector>

#include <oni-core/common/oni-common-typedefs-graphic.h>

namespace oni {
    class IndexBuffer {
    public:
        explicit IndexBuffer(oniGLuint count,
                             oniGLsizei size = 1);

        IndexBuffer() = default;

        ~IndexBuffer();

        void
        bind() const;

        void
        unbind() const;

    private:
        oniGLuint mBufferID;
        oniGLuint mCount;
        oniGLsizei mSize;
    };
}
