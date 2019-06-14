#include <oni-core/graphic/oni-graphic-renderer-ogl.h>

#include <algorithm>
#include <cassert>

#include <GL/glew.h>

#include <oni-core/graphic/buffer/oni-graphic-buffer.h>
#include <oni-core/graphic/buffer/oni-graphic-index-buffer.h>
#include <oni-core/graphic/buffer/oni-graphic-vertex-array.h>
#include <oni-core/graphic/oni-graphic-texture-manager.h>


namespace oni {
    namespace graphic {
        Renderer_OpenGL::Renderer_OpenGL(PrimitiveType primitiveType) : mPrimitiveType(primitiveType) {}

        Renderer_OpenGL::~Renderer_OpenGL() = default;

        common::oniGLint
        Renderer_OpenGL::getSamplerID(common::oniGLuint textureID) {
            auto it = std::find(mTextures.begin(), mTextures.end(), textureID);
            if (it == mTextures.end()) {
                /*
                 * This can be used to render more than max number of samplers, but it slows things down.
                if (mNextSamplerID > mMaxNumTextureSamplers) {
                    reset();
                }*/
                assert(mNextSamplerID <= mMaxNumTextureSamplers);

                mTextures.push_back(textureID);
                mSamplers.push_back(mNextSamplerID++);
                return mSamplers.back();
            } else {
                auto index = std::distance(mTextures.begin(), it);
                return mSamplers[index];
            }
        }

        void
        Renderer_OpenGL::_begin(const math::mat4 &model,
                                const math::mat4 &view,
                                const math::mat4 &proj) {
            enableShader(model, view, proj);

            mNextSamplerID = 0;
            mSamplers.clear();
            mTextures.clear();

            bindVertexBuffer();
            // Data written to mBuffer has to match the structure of VBO.
            mBuffer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        }

        void
        Renderer_OpenGL::_end() {
            glUnmapBuffer(GL_ARRAY_BUFFER);
            unbindVertexBuffer();
        }

        void
        Renderer_OpenGL::_flush() {
            TextureManager::bindRange(0, mTextures);

            bindVertexArray();
            bindIndexBuffer();

            auto indexCount = getIndexCount();

            switch (mPrimitiveType) {
                case PrimitiveType::POINT: {
                    glDrawArrays(GL_POINTS, 0, indexCount);
                    break;
                }
                case PrimitiveType::LINE: {
                    glDrawArrays(GL_LINES, 0, indexCount);
                    break;
                }
                case PrimitiveType::TRIANGLE: {
                    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
                    break;
                }
                case PrimitiveType::TRIANGLE_STRIP: {
                    glDrawArrays(GL_TRIANGLE_STRIP, 0, indexCount);
                    break;
                }
                default: {
                    assert(false);
                }
            }

            unbindIndexBuffer();
            unbindVertexArray();

            TextureManager::unbind();

            resetIndexCount();

            disableShader();
        }
    }
}