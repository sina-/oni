#include <oni-core/graphic/buffer/oni-graphic-buffer.h>

#include <GL/glew.h>

#include <oni-core/graphic/buffer/oni-graphic-buffer-data.h>
#include <oni-core/graphic/buffer/oni-graphic-vertex-array.h>

namespace oni {
    VertexArray::VertexArray(
            const std::vector<BufferStructure> &bufferStructure,
            oniGLsizei maxBufferSize) : mMaxBufferSize{maxBufferSize} {
        mVertexBuffers = std::make_unique<Buffer>(std::vector<oniGLfloat>(), mMaxBufferSize,
                                                  GL_STATIC_DRAW);
        mBufferStructure = bufferStructure;
        glGenVertexArrays(1, &mArrayID);

        bindVAO();
        mVertexBuffers->bind();

        for (auto &&vertex: mBufferStructure) {
            glEnableVertexAttribArray(vertex.index);
            glVertexAttribPointer(vertex.index, vertex.componentCount, vertex.componentType, vertex.normalized,
                                  vertex.stride, vertex.offset);
        }

        mVertexBuffers->unbind();
        unbindVAO();
    }

    VertexArray::~VertexArray() {
        glDeleteVertexArrays(1, &mArrayID);
    }

    void
    VertexArray::bindVAO() const {
        glBindVertexArray(mArrayID);
    }

    void
    VertexArray::unbindVAO() const {
        glBindVertexArray(0);
    }

    void
    VertexArray::bindVBO() const {
        mVertexBuffers->bind();
    }

    void
    VertexArray::unbindVBO() const {
        mVertexBuffers->unbind();
    }
}
