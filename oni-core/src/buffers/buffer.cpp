#include "buffers/buffer.h"

namespace oni {
    namespace buffers {
        Buffer::Buffer(const std::vector<GLfloat> &data, GLuint componentCount, GLsizei size)
                : m_Size(size), m_ComponentCount(componentCount) {
            auto dataSize = data.size() * sizeof(GLfloat);
            glGenBuffers(size, &m_BufferID);
            glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
            glBufferData(GL_ARRAY_BUFFER, dataSize, data.data(), GL_STATIC_DRAW);

            GLint actualSize = 0;
            glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &actualSize);
            if (dataSize != actualSize) {
                glDeleteBuffers(m_Size, &m_BufferID);
                throw std::runtime_error("Could not allocate vertex buffer!");
            }

            unbind();

        }
    }
}
