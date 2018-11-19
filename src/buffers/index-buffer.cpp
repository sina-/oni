#include <oni-core/buffers/index-buffer.h>

#include <stdexcept>

namespace oni {
    namespace buffers {
        IndexBuffer::IndexBuffer(const std::vector<GLuint> &data, GLuint count, GLsizei size) : mBufferID{0},
                                                                                                mCount{count},
                                                                                                mSize{size} {
            auto dataSize = count * sizeof(GLuint);

            glGenBuffers(size, &mBufferID);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferID);
            // NOTE: dataSize type should match the enum passed to draw call!
            // Otherwise its UB and you get garbage rendered.
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize, data.data(), GL_STATIC_DRAW);

            GLint actualSize{0};
            glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &actualSize);
            if (static_cast<GLint>(dataSize) != actualSize) {
                glDeleteBuffers(mSize, &mBufferID);
                throw std::runtime_error("Could not allocate index buffer!");
            }

            unbind();

        }

        IndexBuffer::~IndexBuffer() {
            glDeleteBuffers(mSize, &mBufferID);
        }

        void IndexBuffer::bind() const {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferID);
        }

        void IndexBuffer::unbind() const {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        GLuint IndexBuffer::getCount() const {
            return mCount;
        }

    }
}
