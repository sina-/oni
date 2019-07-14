#pragma once

#include <oni-core/graphic/oni-graphic-renderer-ogl.h>

namespace oni {
    namespace graphic {
        class Shader;

        class Renderer_OpenGL_Quad : public Renderer_OpenGL {
        public:
            explicit Renderer_OpenGL_Quad(common::oniGLsizei maxSpriteCount);

            ~Renderer_OpenGL_Quad() override;

            Renderer_OpenGL_Quad(const Renderer_OpenGL_Quad &) = delete;

            Renderer_OpenGL_Quad &
            operator=(Renderer_OpenGL_Quad &) = delete;

            void
            submit(const component::WorldP3D *,
                   const component::Scale &,
                   const component::Color &,
                   const component::Texture &);

            void
            setFrameBufferTexture(common::oniGLint textureID);

        protected:
            void
            enableShader(const math::mat4 &model,
                         const math::mat4 &view,
                         const math::mat4 &proj,
                         const math::vec2 &screenSize,
                         common::r32 zoom) override;

            common::oniGLsizei
            getIndexCount() override;

            void
            resetIndexCount() override;

            void
            attachFrameBuffer() override;

        private:
            common::oniGLsizei mMaxPrimitiveCount{0};
            common::oniGLsizei mMaxIndicesCount{0};

            // Actual number of indices used.
            common::oniGLsizei mIndexCount{0};

            common::oniGLuint mFBOTextureID{0};
        };
    }
}
