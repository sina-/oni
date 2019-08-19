#pragma once

#include <oni-core/common/oni-common-typedefs-graphic.h>
#include <oni-core/component/oni-component-fwd.h>

namespace oni {
    class FrameBuffer {
    public:
        FrameBuffer();

        ~FrameBuffer();

        void
        bind();

        static void
        unbind();

        void
        attach(Texture &renderTarget);

        static void
        clear();

    private:
        static void
        checkFBO();

    private:
        oniGLuint mBufferID{};
        oniGLuint mTextureID{};
    };
}
