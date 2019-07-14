#pragma once

#include <vector>

#include <oni-core/common/oni-common-typedefs-graphic.h>

namespace oni {
    namespace graphic {
        class IndexBuffer {
        public:
            explicit IndexBuffer(common::oniGLuint count,
                                 common::oniGLsizei size = 1);

            IndexBuffer() = default;

            ~IndexBuffer();

            void
            bind() const;

            void
            unbind() const;

        private:
            common::oniGLuint mBufferID;
            common::oniGLuint mCount;
            common::oniGLsizei mSize;
        };

    }
}
