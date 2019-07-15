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

            common::oniGLuint
            getFrameBufferTextureID();

            void
            attach(common::oniGLuint textureID);

        private:
            static void checkFBO();

        private:
            common::oniGLuint mBufferID{};
            common::oniGLuint mTextureID{};
        };
    }
}

