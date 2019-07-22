#pragma once

#include <oni-core/graphic/oni-graphic-renderer-ogl.h>
#include <oni-core/component/oni-component-geometry.h>

namespace oni {
    namespace graphic {
        class Shader;

        class Renderer_OpenGL_Strip : public Renderer_OpenGL {
        public:
            explicit Renderer_OpenGL_Strip(common::oniGLsizei maxSpriteCount);

            ~Renderer_OpenGL_Strip() override;

            void
            submit(const Renderable &renderable) override;

            void
            submit(const component::WorldP3D &,
                   const common::r32 offset,
                   const component::Color &color,
                   const component::Texture &texture);

            void
            setStripeSize(common::r32);

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

        private:
            common::oniGLsizei mMaxPrimitiveCount{0};
            common::oniGLsizei mVertexSize{0};
            common::oniGLsizei mMaxIndicesCount{0};
            common::oniGLsizei mPrimitiveSize{0};

            common::oniGLsizei mIndexCount{0};

            common::r32 mStripeSize{0.4f};
        };
    }
}

