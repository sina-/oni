#include <oni-core/graphic/buffer/oni-graphic-frame-buffer.h>

#include <GL/glew.h>
#include <cassert>
#include <oni-core/component/oni-component-visual.h>

namespace oni {
    namespace graphic {
        FrameBuffer::FrameBuffer() {
            glGenFramebuffers(1, &mBufferID);
        }

        FrameBuffer::~FrameBuffer() {
            glDeleteFramebuffers(1, &mBufferID);
        }

        void
        FrameBuffer::checkFBO() {
            auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            assert(status == GL_FRAMEBUFFER_COMPLETE);
        }

        void
        FrameBuffer::bind() {
            glBindFramebuffer(GL_FRAMEBUFFER, mBufferID);
        }

        void
        FrameBuffer::unbindAndClear() {
            common::oniGLuint color[4] = {0, 0, 0, 0};
#if 0
            // TODO: This is OGL 4.0+
            glClearTexImage(mTextureID, 0, GL_BGRA, GL_UNSIGNED_BYTE, &color);
#else
            glClearBufferuiv(GL_COLOR, 0, color);
#endif
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void
        FrameBuffer::attach(component::Texture &renderTarget) {
            auto textureID = renderTarget.textureID;
            if (mTextureID != textureID) {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
                checkFBO();
                mTextureID = textureID;
            }
        }
    }
}