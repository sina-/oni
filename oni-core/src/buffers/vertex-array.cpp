#include "buffers/vertex-array.h"

namespace oni {
    namespace buffers {
        VertexArray::VertexArray(std::unique_ptr<const Buffer> vertexBuffer) {
            glGenVertexArrays(1, &mArrayID);

            mVertexBuffers = std::move(vertexBuffer);

            bindVAO();
            mVertexBuffers->bind();

            for (const auto &vertex: *(mVertexBuffers->getBufferStructure())) {
                glEnableVertexAttribArray(vertex->index);
                glVertexAttribPointer(vertex->index, vertex->componentCount, vertex->componentType, vertex->normalized,
                                      vertex->stride, vertex->offset);
            }

            mVertexBuffers->unbind();
            unbindVAO();
        }

    }
}
