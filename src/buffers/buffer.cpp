#include <oni-core/utils/oni-assert.h>
#include <oni-core/buffers/buffer.h>

namespace oni {
    namespace buffers {
        Buffer::Buffer(const std::vector<GLfloat> &data, GLsizeiptr dataSize, GLenum usage,
                       components::BufferStructures bufferStructures)
                : mBufferStructures(std::move(bufferStructures)) {
            // Check for supported usages.
            ONI_DEBUG_ASSERT(usage == GL_STATIC_DRAW || usage == GL_DYNAMIC_DRAW)

            glGenBuffers(1, &mBufferID);
            bind();
            // Use the data for GL_STATIC_DRAW, otherwise pass nullptr for GL_DYNAMIC_DRAW.
            auto dataPtr = !data.empty() ? data.data() : nullptr;
            glBufferData(GL_ARRAY_BUFFER, dataSize, dataPtr, usage);

            GLint actualSize = 0;
            glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &actualSize);
            if (dataSize != actualSize) {
                glDeleteBuffers(1, &mBufferID);
                throw std::runtime_error("Could not allocate vertex buffer!");
            }

            unbind();

        }
    }
}
