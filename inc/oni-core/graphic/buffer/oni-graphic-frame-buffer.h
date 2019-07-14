#pragma once

#include <oni-core/common/oni-common-typedefs-graphic.h>

namespace oni {
    namespace graphic {
        class FrameBuffer {
        public:
            FrameBuffer();

            ~FrameBuffer();

            void
            bind();

            static void
            unbind();

            static void
            attach(common::oniGLint);

        private:
            common::oniGLuint mBufferID{};
        };
    }
}

