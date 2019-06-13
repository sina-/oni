#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/graphic/oni-graphic-renderer-opengl.h>

namespace oni {
    namespace component {
        struct Texture;

        struct Text;

        struct Appearance;
    }

    namespace graphic {
        class Shader;

        class Tessellation_Renderer_OpenGL : public Renderer_OpenGL {
        public:
            explicit Tessellation_Renderer_OpenGL(common::oniGLsizei maxSpriteCount);

            ~Tessellation_Renderer_OpenGL() override;

            Tessellation_Renderer_OpenGL(const Tessellation_Renderer_OpenGL &) = delete;

            Tessellation_Renderer_OpenGL &
            operator=(Tessellation_Renderer_OpenGL &) = delete;

        private:
            void
            _submit(const component::WorldP3D &,
                    const component::Heading &,
                    const component::Scale &,
                    const component::Appearance &,
                    const component::Texture &) override;

            void
            _submit(const component::Text &,
                    const component::WorldP3D &) override;

            void
            enableShader(const math::mat4 &model,
                         const math::mat4 &view,
                         const math::mat4 &proj) override;

            void
            disableShader() override;

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