#pragma once

#include <vector>

#include <oni-core/common/typedefs-graphics.h>

namespace oni {
    namespace buffer {
        class IndexBuffer {
        public:
            IndexBuffer(const std::vector<common::oniGLuint> &data, common::oniGLuint count, common::oniGLsizei size = 1);

            IndexBuffer() = default;

            ~IndexBuffer();

            void bind() const;

            void unbind() const;

        private:
            common::oniGLuint mBufferID;
            common::oniGLuint mCount;
            common::oniGLsizei mSize;
        };

    }
}
