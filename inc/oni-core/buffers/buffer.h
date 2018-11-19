#pragma once

#include <vector>

#include <GL/glew.h>

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace buffers {

        /*
         * Great tutorial on OpenGL buffers: https://open.gl/drawing
         */
        class Buffer {
            GLuint mBufferID;
            // TODO: This is un-used by the Buffer class, but VAO which usually has this class uses it.
            common::BufferStructures mBufferStructures;

        public:
            Buffer(const std::vector<GLfloat> &data, GLsizeiptr dataSize, GLenum usage,
                   common::BufferStructures bufferStructures);

            ~Buffer();

            Buffer &operator=(const Buffer &) = delete;

            Buffer &operator=(Buffer &) = delete;

            Buffer(const Buffer &) = delete;

            Buffer(Buffer &) = delete;

            void bind();

            void unbind();

            const common::BufferStructures &getBufferStructure() const;
        };

    }
}
