#pragma once

#include <GL/glew.h>
#include <stdexcept>
#include <vector>

namespace oni {
    namespace buffers {
        class IndexBuffer {
            GLuint mBufferID;
            GLuint mCount;
            GLsizei mSize;

        public:
            IndexBuffer(const std::vector<GLuint> &data, GLuint count, GLsizei size = 1);

            IndexBuffer() = default;

            ~IndexBuffer() { glDeleteBuffers(mSize, &mBufferID); }

            inline void bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferID); }

            inline void unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

            inline GLuint getCount() const { return mCount; }
        };

    }
}
