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
            auto indexCount = getIndexCount();
            if (indexCount < 1) {
                return;
            }
            TextureManager::bindRange(0, mTextures);

            bindVertexArray();
            bindIndexBuffer();

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
                    // NOTE: This buffer uses last two and next two vertices to decide on the direction,
                    // so I have to always draw 4 items less other wise I will access out of bounds in the shader
                    if (indexCount > 4) {
                        glDrawArrays(GL_TRIANGLE_STRIP, 0, indexCount - 4);
                    }
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