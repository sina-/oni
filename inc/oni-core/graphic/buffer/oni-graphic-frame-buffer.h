#pragma once

#include <oni-core/common/oni-common-typedefs-graphic.h>

namespace oni {
    namespace component {
        struct Texture;
    }
    namespace graphic {

        class FrameBuffer {
        public:
            FrameBuffer();

            ~FrameBuffer();

            void
            bind();

            static void
            unbindAndClear();

            void
            attach(component::Texture &renderTarget);

        private:
            static void
            checkFBO();

        private:
            common::oniGLuint mBufferID{};
            common::oniGLuint mTextureID{};
        };
    }
}

