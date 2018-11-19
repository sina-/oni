#pragma once

#include <vector>

#include <GL/glew.h>

namespace oni {
    namespace buffers {
        class IndexBuffer {
        public:
            IndexBuffer(const std::vector<GLuint> &data, GLuint count, GLsizei size = 1);

            IndexBuffer() = default;

            ~IndexBuffer();

            void bind() const;

            void unbind() const;

            GLuint getCount() const;

        private:
            GLuint mBufferID;
            GLuint mCount;
            GLsizei mSize;
        };

    }
}
