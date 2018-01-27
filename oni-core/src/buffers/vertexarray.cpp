#include <graphics/renderable2d.h>
#include "buffers/vertexarray.h"

namespace oni {
    namespace buffers {
        VertexArray::VertexArray() {
            glGenVertexArrays(1, &m_ArrayID);
        }

        void VertexArray::addBuffer(std::unique_ptr<const Buffer> vertexBuffer) {
            m_VertexBuffers = std::move(vertexBuffer);

            bindVAO();
            m_VertexBuffers->bind();

            for (const auto &vertex: *(m_VertexBuffers->getBufferStructure())) {
                glEnableVertexAttribArray(vertex->index);
                glVertexAttribPointer(vertex->index, vertex->componentCount, vertex->componentType, vertex->normalized,
                                      vertex->stride, vertex->offset);
            }

            m_VertexBuffers->unbind();
            unbindVAO();
        }

    }
}
