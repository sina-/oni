#pragma once

#include <memory>
#include <vector>

#include <oni-core/common/oni-common-typedefs-graphic.h>
#include <oni-core/graphic/oni-graphic-renderer.h>


namespace oni {
    namespace buffer {
        class IndexBuffer;

        class VertexArray;
    }

    namespace graphic {
        class Shader;

        class Renderer_OpenGL : public Renderer {
        public:
            explicit Renderer_OpenGL(PrimitiveType);

            ~Renderer_OpenGL() override;

        protected:
            void
            _begin(const math::mat4 &model,
                   const math::mat4 &view,
                   const math::mat4 &proj,
                   const math::vec2 &screenSize,
                   common::r32 zoom) override;

            void
            _flush() override;

            void
            _end() override;

        protected:
            virtual void
            enableShader(const math::mat4 &model,
                         const math::mat4 &view,
                         const math::mat4 &proj,
                         const math::vec2 &screenSize,
                         common::r32 zoom) = 0;

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

            virtual common::oniGLsizei
            getIndexCount() = 0;

            virtual void
            resetIndexCount() = 0;

        protected:
            void *mBuffer{nullptr};

            // TODO: Arbitrary number, query the graphics card for it
            common::oniGLint mMaxNumTextureSamplers{32};

            std::unique_ptr<Shader> mShader{};
            std::unique_ptr<buffer::VertexArray> mVertexArray{nullptr};
            std::unique_ptr<buffer::IndexBuffer> mIndexBuffer{nullptr};

        private:
            PrimitiveType mPrimitiveType{PrimitiveType::UNKNOWN};

            common::oniGLint mNextSamplerID{0};

            std::vector<common::oniGLint> mSamplers{};
            std::vector<common::oniGLuint> mTextures{};
        };
    }
}
