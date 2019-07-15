#include <oni-core/graphic/buffer/oni-graphic-frame-buffer.h>

#include <GL/glew.h>
#include <cassert>

namespace oni {
    namespace graphic {
        FrameBuffer::FrameBuffer() {
            glGenFramebuffers(1, &mBufferID);
            {
                glGenTextures(1, &mTextureID);

                assert(mTextureID);

                glBindTexture(GL_TEXTURE_2D, mTextureID);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 400, 400, 0, GL_BGRA, GL_UNSIGNED_BYTE, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            attach(mTextureID);
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
        FrameBuffer::clearBuffer() {
            common::oniGLuint color[4] = {0, 0, 0, 0};
#if 0
            // TODO: This is OGL 4.0+
            glClearTexImage(mTextureID, 0, GL_BGRA, GL_UNSIGNED_BYTE, &color);
#else
            bind();
            glClearBufferuiv(GL_COLOR, 0, color);
            unbind();
#endif
        }

        void
        FrameBuffer::bind() {
            glBindFramebuffer(GL_FRAMEBUFFER, mBufferID);
        }

        void
        FrameBuffer::unbind() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        common::oniGLuint
        FrameBuffer::getFrameBufferTextureID() {
            return mTextureID;
        }

        void
        FrameBuffer::attach(common::oniGLuint textureID) {
            bind();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
            checkFBO();
            unbind();
        }
    }
}