#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/graphic/oni-graphic-renderer-ogl.h>

namespace oni {
    namespace component {
        struct Texture;

        struct Text;

        struct Color;
    }

    namespace graphic {
        class Shader;

        class Renderer_OpenGL_Tessellation : public Renderer_OpenGL {
        public:
            explicit Renderer_OpenGL_Tessellation(common::oniGLsizei maxSpriteCount);

            ~Renderer_OpenGL_Tessellation() override;

            Renderer_OpenGL_Tessellation(const Renderer_OpenGL_Tessellation &) = delete;

            Renderer_OpenGL_Tessellation &
            operator=(Renderer_OpenGL_Tessellation &) = delete;

            void
            submit(const component::WorldP3D &,
                   const component::Heading &,
                   const component::Scale &,
                   const component::Color &,
                   const component::Texture &);

            void
            submit(const component::Text &,
                   const component::WorldP3D &);

        protected:
            common::oniGLsizei
            getIndexCount() override;

            void
            resetIndexCount() override;

            void
            enableShader(const math::mat4 &model,
                         const math::mat4 &view,
                         const math::mat4 &proj,
                         const math::vec2 &screenSize,
                         common::r32 zoom) override;

        private:
            common::oniGLsizei mMaxPrimitiveCount{0};
            common::oniGLsizei mMaxIndicesCount{0};

            // Actual number of indices used.
            common::oniGLsizei mIndexCount{0};
        };

    }
}