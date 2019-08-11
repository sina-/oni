#pragma once

#include <memory>
#include <vector>

#include <oni-core/common/oni-common-typedefs-graphic.h>
#include <oni-core/graphic/buffer/oni-graphic-frame-buffer.h>
#include <oni-core/graphic/buffer/oni-graphic-buffer.h>
#include <oni-core/graphic/buffer/oni-graphic-index-buffer.h>
#include <oni-core/graphic/buffer/oni-graphic-vertex-array.h>
#include <oni-core/graphic/oni-graphic-renderer.h>


namespace oni {
    namespace graphic {
        class Shader;

        class Renderer_OpenGL : public Renderer {
        public:
            explicit Renderer_OpenGL(PrimitiveType);

            ~Renderer_OpenGL() override;

        protected:
            void
            _begin(const RenderSpec &spec,
                   const BlendSpec &,
                   const DepthSpec &) override;

            void
            _flush(component::Texture *renderTarget) override;

            void
            _end() override;

            void
            setViewportSize(const WindowSize &size) override;

            WindowSize
            getViewportSize() override;

        protected:
            virtual void
            enableShader(const RenderSpec &) = 0;

            virtual void
            disableShader();

            // TODO: checkout texture arrays.
            /**
             * There are 0, 1, ..., mMaxNumTextureSamplers texture samplers available.
             * Each texture is assigned one and the id to the sampler is saved as part of vertex data
             * in the vertex buffer. During rendering in the shader the proper sampler is selected based
             * on the sampler id in the buffer.
             */
            common::oniGLint
            getSamplerID(common::oniGLuint textureID);

            virtual void
            bindVertexArray();

            virtual void
            unbindVertexArray();

            virtual void
            bindVertexBuffer();

            virtual void
            unbindVertexBuffer();

            virtual void
            bindIndexBuffer();

            virtual void
            unbindIndexBuffer();

            virtual void
            bindFrameBuffer();

            virtual void
            unbindFrameBuffer();

            virtual void
            attachFrameBuffer(component::Texture &renderTarget);

            virtual void
            clearFrameBuffer();

            virtual common::oniGLsizei
            getIndexCount() = 0;

            virtual void
            resetIndexCount() = 0;
        private:
            static common::oniGLenum getBlendMode(BlendMode);

        protected:
            void *mBuffer{nullptr};

            // TODO: Arbitrary number, query the graphics card for it
            common::oniGLint mMaxNumTextureSamplers{32};

            std::unique_ptr<Shader> mShader{};
            std::unique_ptr<VertexArray> mVertexArray{nullptr};
            std::unique_ptr<IndexBuffer> mIndexBuffer{nullptr};
            std::unique_ptr<FrameBuffer> mFrameBuffer{nullptr};

        private:
            PrimitiveType mPrimitiveType{PrimitiveType::UNKNOWN};

            common::oniGLint mNextSamplerID{0};

            std::vector<common::oniGLint> mSamplers{};
            std::vector<common::oniGLuint> mTextures{};
        };
    }
}
