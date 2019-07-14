#include <oni-core/graphic/buffer/oni-graphic-frame-buffer.h>

#include <GL/glew.h>

namespace oni {
    namespace graphic {
        FrameBuffer::FrameBuffer() {
            glGenFramebuffers(1, &mBufferID);
        }

        FrameBuffer::~FrameBuffer() {
            glDeleteFramebuffers(1, &mBufferID);
        }

        void
        FrameBuffer::bind() {
            glBindFramebuffer(GL_FRAMEBUFFER, mBufferID);
        }

        void
        FrameBuffer::unbind() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void
        FrameBuffer::attach(common::oniGLint textureID) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
        }
    }
}