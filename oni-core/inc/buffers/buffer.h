#pragma once

#include <GL/glew.h>
#include <stdexcept>
#include <vector>
#include <components/visual.h>
#include <components/buffer.h>

namespace oni {
    namespace buffers {

        /*
         * Great tutorial on OpenGL buffers: https://open.gl/drawing
         */
        class Buffer {
            GLuint mBufferID;
            components::BufferStructures mBufferStructures;

        public:
            Buffer(const std::vector<GLfloat> &data, GLsizeiptr dataSize, GLenum usage,
                   components::BufferStructures bufferStructures);

            ~Buffer() { glDeleteBuffers(1, &mBufferID); }

            Buffer &operator=(const Buffer &) = delete;

            Buffer &operator=(Buffer &) = delete;

            inline void bind() const { glBindBuffer(GL_ARRAY_BUFFER, mBufferID); }

            inline void unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

            inline const auto getBufferStructure() const { return &mBufferStructures; }
        };

    }
}
