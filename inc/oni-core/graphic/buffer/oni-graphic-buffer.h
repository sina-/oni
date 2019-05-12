#pragma once

#include <vector>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/common/oni-common-typedefs-graphic.h>

namespace oni {
    namespace buffer {

        /*
         * Great tutorial on OpenGL buffers: https://open.gl/drawing
         */
        class Buffer {
        public:
            Buffer(const std::vector<common::oniGLfloat> &data,
                   common::oniGLsizeiptr dataSize,
                   common::oniGLenum usage);

            ~Buffer();

            Buffer &
            operator=(const Buffer &) = delete;

            Buffer &
            operator=(Buffer &) = delete;

            Buffer(const Buffer &) = delete;

            Buffer(Buffer &) = delete;

            void
            bind();

            void
            unbind();

        private:
            common::oniGLuint mBufferID;
        };

    }
}
