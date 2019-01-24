#include <oni-core/buffer/buffer.h>

#include <GL/glew.h>

#include <oni-core/component/buffer.h>
#include <oni-core/buffer/vertex-array.h>

namespace oni {
    namespace buffer {
        VertexArray::VertexArray(std::unique_ptr<Buffer> vertexBuffer) {
            glGenVertexArrays(1, &mArrayID);

            mVertexBuffers = std::move(vertexBuffer);

            bindVAO();
            mVertexBuffers->bind();

            for (auto &&vertex: mVertexBuffers->getBufferStructure()) {
                glEnableVertexAttribArray(vertex->index);
                glVertexAttribPointer(vertex->index, vertex->componentCount, vertex->componentType, vertex->normalized,
                                      vertex->stride, vertex->offset);
            }

            mVertexBuffers->unbind();
            unbindVAO();
        }

        VertexArray::~VertexArray() {
            glDeleteVertexArrays(1, &mArrayID);
        }

        void VertexArray::bindVAO() const {
            glBindVertexArray(mArrayID);
        }

        void VertexArray::unbindVAO() const {
            glBindVertexArray(0);
        }

        void VertexArray::bindVBO() const {
            mVertexBuffers->bind();
        }

        void VertexArray::unbindVBO() const {
            mVertexBuffers->unbind();
        }

    }
}
