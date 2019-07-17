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
            unbind();

            void
            attach(component::Texture &renderTarget);

            static void
            clear();

        private:
            static void
            checkFBO();

        private:
            common::oniGLuint mBufferID{};
            common::oniGLuint mTextureID{};
        };
    }
}

