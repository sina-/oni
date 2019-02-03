#pragma once

#include <map>

#include <oni-core/graphic/renderer-2d.h>
#include <oni-core/component/buffer.h>
#include <oni-core/common/typedefs.h>
#include <oni-core/component/buffer.h>

namespace oni {
    namespace buffer {
        class IndexBuffer;

        class VertexArray;
    }

    namespace component {
        class Texture;

        class Shape;

        class Text;

        class Appearance;
    }

    namespace graphic {

        class BatchRenderer2D : public Renderer2D {
        public:
            BatchRenderer2D(common::oniGLsizei maxSpriteCount, common::oniGLsizei maxNumTextureSamplers,
                            common::oniGLsizei maxVertexSize,
                            std::vector<component::BufferStructure> bufferStructures);

            ~BatchRenderer2D() override;

            BatchRenderer2D(const BatchRenderer2D &) = delete;

            BatchRenderer2D &operator=(BatchRenderer2D &) = delete;

            std::vector<common::oniGLint> generateSamplerIDs();

        private:
            void _begin() override;

            void _submit(const component::Shape &position, const component::Appearance &color) override;

            void _submit(const component::Shape &position, const component::Texture &texture) override;

            void _submit(const component::Text &text) override;

            void _flush() override;

            void _end() override;

            void reset();

            // TODO: checkout texture arrays.
            /**
             * There are 0, 1, ..., mMaxNumTextureSamplers texture samplers available.
             * Each texture is assigned one and the id to the sampler is saved as part of vertex data
             * in the vertex buffer. During rendering in the shader the proper sampler is selected based
             * on the sampler id in the buffer.
             */
            common::oniGLint getSamplerID(common::oniGLuint textureID);

        private:
            // Actual number of indices used.
            common::oniGLsizei mIndexCount;

            common::oniGLsizei mMaxSpriteCount;
            common::oniGLsizei mMaxVertexSize;
            const common::oniGLint mMaxNumTextureSamplers;
            common::oniGLsizei mMaxIndicesCount;
            common::oniGLsizei mMaxSpriteSize;
            common::oniGLsizei mMaxBufferSize;

            common::oniGLuint mVDO;

            std::unique_ptr<buffer::VertexArray> mVAO;
            std::unique_ptr<buffer::IndexBuffer> mIBO;

            common::oniGLint mNextSamplerID{0};
            std::vector<common::oniGLint> mSamplers{};
            std::vector<common::oniGLuint> mTextures{};

            // The buffer that will hold component::Vertex data, or its variants, in the batch.
            void *mBuffer;
        };

    }
}