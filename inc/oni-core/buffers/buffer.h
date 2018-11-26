#pragma once

#include <vector>

#include <oni-core/common/typedefs.h>
#include <oni-core/common/typedefs-graphics.h>

namespace oni {
    namespace components {
        struct BufferStructure;
        typedef std::vector<std::unique_ptr<components::BufferStructure>> BufferStructureList;
    }

    namespace buffers {

        /*
         * Great tutorial on OpenGL buffers: https://open.gl/drawing
         */
        class Buffer {
        public:
            Buffer(const std::vector<common::oniGLfloat> &data, common::oniGLsizeiptr dataSize, common::oniGLenum usage,
                   components::BufferStructureList bufferStructures);

            ~Buffer();

            Buffer &operator=(const Buffer &) = delete;

            Buffer &operator=(Buffer &) = delete;

            Buffer(const Buffer &) = delete;

            Buffer(Buffer &) = delete;

            void bind();

            void unbind();

            const components::BufferStructureList &getBufferStructure() const;

        private:
            common::oniGLuint mBufferID;
            // TODO: This is un-used by the Buffer class, but VAO which usually has this class uses it.
            components::BufferStructureList mBufferStructureList;

        };

    }
}
