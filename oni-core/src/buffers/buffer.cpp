#include <utils/oni_assert.h>
#include <buffers/buffer.h>

namespace oni {
    namespace buffers {
        Buffer::Buffer(const std::vector<GLfloat> &data, GLsizeiptr dataSize, GLenum usage,
                       components::BufferStructures bufferStructures)
                : m_BufferStructures(std::move(bufferStructures)) {
            // Check for supported usages.
            ONI_DEBUG_ASSERT(usage == GL_STATIC_DRAW || usage == GL_DYNAMIC_DRAW)

            glGenBuffers(1, &m_BufferID);
            bind();
            auto dataPtr = !data.empty() ? data.data() : nullptr;
            glBufferData(GL_ARRAY_BUFFER, dataSize, dataPtr, usage);

            GLint actualSize = 0;
            glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &actualSize);
            if (dataSize != actualSize) {
                glDeleteBuffers(1, &m_BufferID);
                throw std::runtime_error("Could not allocate vertex buffer!");
            }

            unbind();

        }
    }
}
