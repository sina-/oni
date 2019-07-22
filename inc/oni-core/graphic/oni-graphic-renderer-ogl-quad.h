#pragma once

#include <oni-core/graphic/oni-graphic-renderer-ogl.h>

namespace oni {
    namespace graphic {
        class Shader;

        class Renderer_OpenGL_Quad : public Renderer_OpenGL {
        public:
            explicit Renderer_OpenGL_Quad(common::oniGLsizei maxSpriteCount);

            ~Renderer_OpenGL_Quad() override;

            void
            submit(const Renderable &renderable) override;

            // TODO: These are only used to render to texture, so there is no need to unify it at the moment.
            void
            submit(const component::Quad &,
                   const component::Color &,
                   const component::Texture *);

            void
            submit(const component::Quad &,
                   const component::Color &,
                   const component::Texture &front,
                   const component::Texture &back);

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
            common::oniGLsizei mMaxIndicesCount{0};

            // Actual number of indices used.
            common::oniGLsizei mIndexCount{0};
        };
    }
}
