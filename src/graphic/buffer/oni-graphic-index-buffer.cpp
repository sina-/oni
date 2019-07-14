#include <oni-core/graphic/buffer/oni-graphic-index-buffer.h>

#include <assert.h>

#include <GL/glew.h>

namespace oni {
    namespace graphic {
        IndexBuffer::IndexBuffer(common::oniGLuint count,
                                 common::oniGLsizei size) : mBufferID{0},
                                                            mCount{count},
                                                            mSize{size} {

            std::vector<common::oniGLuint> indices(mCount);
            common::oniGLushort offset = 0;
            for (auto i = 0; i < mCount; i += 6) {
                indices[i + 0] = offset + 0;
                indices[i + 1] = offset + 1;
                indices[i + 2] = offset + 2;

                indices[i + 3] = offset + 2;
                indices[i + 4] = offset + 3;
                indices[i + 5] = offset + 0;

                offset += 4;
            }
            auto dataSize = mCount * sizeof(GLuint);

            glGenBuffers(size, &mBufferID);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferID);
            // NOTE: dataSize type should match the enum passed to draw call!
            // Otherwise its UB and you get garbage rendered.
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize, indices.data(), GL_STATIC_DRAW);

            GLint actualSize{0};
            glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &actualSize);
            if (static_cast<GLint>(dataSize) != actualSize) {
                glDeleteBuffers(mSize, &mBufferID);
                assert(false);
            }

            unbind();

        }

        IndexBuffer::~IndexBuffer() {
            glDeleteBuffers(mSize, &mBufferID);
        }

        void
        IndexBuffer::bind() const {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferID);
        }

        void
        IndexBuffer::unbind() const {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    }
}
