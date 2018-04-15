#pragma once

#include <map>

#include <oni-core/graphics/renderer-2d.h>
#include <oni-core/components/buffer.h>
#include <oni-core/common/typedefs.h>

namespace oni {
    namespace buffers {
        class IndexBuffer;

        class VertexArray;
    }

    namespace components {
        class Texture;
        class Placement;
        class Text;
        class Appearance;

    }

    namespace graphics {


        class BatchRenderer2D : public Renderer2D {

            unsigned long mMaxSpriteCount;
            unsigned long mMaxIndicesCount;

            unsigned long mMaxBufferSize;
            GLsizei mMaxVertexSize;
            GLsizei mMaxSpriteSize;

            std::unique_ptr<buffers::IndexBuffer> mIBO;

            // Actual number of indices used.
            GLsizei mIndexCount;

            GLuint mVDO;
            std::unique_ptr<buffers::VertexArray> mVAO;

            // The buffer that will hold components::Vertex data, or its variants, in the batch.
            void *mBuffer;

            // A mapping from texture id to 2D sampler that will draw it.
            std::map<GLuint, GLint> mTextureToSampler;
            std::vector<GLint> mSamplers;

            const unsigned long mMaxNumTextureSamplers;

        public:
            BatchRenderer2D(const unsigned long maxSpriteCount, unsigned long maxNumTextureSamplers,
                            GLsizei maxVertexSize,
                            common::BufferStructures bufferStructures);

            ~BatchRenderer2D();

            void begin() override;

            void submit(const components::Placement &position, const components::Appearance &color) override;

            void submit(const components::Placement &position, const components::Texture &texture) override;

            void submit(const components::Text &text) override;

            void flush() override;

            void end() override;

        private:
            void reset();

            // TODO: checkout texture arrays.
            /**
             * There are 0, 1, ..., mMaxNumTextureSamplers texture samplers are available.
             * Each texture is assigned one and the id to the sampler is saved as part of vertex data
             * in the vertex buffer. During rendering in the shader the proper sampler is selected based
             * on the sampler id in the buffer.
             */
            GLint getSamplerID(GLuint textureID);

        public:
            std::vector<GLint> generateSamplerIDs();
        };

    }
}