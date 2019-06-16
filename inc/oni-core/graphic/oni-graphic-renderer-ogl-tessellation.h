#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/graphic/oni-graphic-renderer-ogl.h>

namespace oni {
    namespace component {
        struct Texture;

        struct Text;

        struct Appearance;
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
                   const component::Appearance &,
                   const component::Texture &);

            void
            submit(const component::Text &,
                   const component::WorldP3D &);

        protected:
            void
            bindVertexArray() override;

            void
            bindIndexBuffer() override;

            void
            unbindVertexArray() override;

            void
            bindVertexBuffer() override;

            void
            unbindVertexBuffer() override;

            void
            unbindIndexBuffer() override;

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

            void
            disableShader() override;


        private:
            common::oniGLsizei mMaxPrimitiveCount{0};
            common::oniGLsizei mVertexSize{0};
            common::oniGLsizei mMaxIndicesCount{0};
            common::oniGLsizei mPrimitiveSize{0};

            // Actual number of indices used.
            common::oniGLsizei mIndexCount{0};

            std::unique_ptr<Shader> mShader{};

            std::unique_ptr<buffer::VertexArray> mVertexArray{nullptr};
            std::unique_ptr<buffer::IndexBuffer> mIndexBuffer{nullptr};
        };

    }
}