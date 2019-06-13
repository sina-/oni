#pragma once

#include <memory>
#include <vector>

#include <oni-core/common/oni-common-typedefs-graphic.h>
#include <oni-core/graphic/oni-graphic-renderer-2d.h>


namespace oni {
    namespace buffer {
        class IndexBuffer;

        class VertexArray;
    }

    namespace graphic {
        class Renderer_OpenGL : public Renderer2D {
        public:
            explicit Renderer_OpenGL(PrimitiveType);

            ~Renderer_OpenGL() override;

        protected:
            void
            _begin(const math::mat4 &model,
                   const math::mat4 &view,
                   const math::mat4 &proj) override;

            void
            _flush() override;

            void
            _end() override;

        protected:
            virtual void
            enableShader(const math::mat4 &model,
                         const math::mat4 &view,
                         const math::mat4 &proj) = 0;

            virtual void
            disableShader() = 0;

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
            bindVertexArray() = 0;

            virtual void
            unbindVertexArray() = 0;

            virtual void
            bindVertexBuffer() = 0;

            virtual void
            unbindVertexBuffer() = 0;

            virtual void
            bindIndexBuffer() = 0;

            virtual void
            unbindIndexBuffer() = 0;

            virtual common::oniGLsizei
            getIndexCount() = 0;

            virtual void
            resetIndexCount() = 0;

        protected:
            void *mBuffer{nullptr};

            // TODO: Arbitrary number, query the graphics card for it
            common::oniGLint mMaxNumTextureSamplers{32};

        private:
            PrimitiveType mPrimitiveType{PrimitiveType::UNKNOWN};

            common::oniGLint mNextSamplerID{0};

            std::vector<common::oniGLint> mSamplers{};
            std::vector<common::oniGLuint> mTextures{};
        };
    }
}
