#pragma once

#include <map>

#include <GL/glew.h>

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

        class Shape;

        class Text;

        class Appearance;
    }

    namespace graphics {

        class BatchRenderer2D : public Renderer2D {
        public:
            BatchRenderer2D(const GLsizei maxSpriteCount, const GLsizei maxNumTextureSamplers,
                            const GLsizei maxVertexSize,
                            components::BufferStructures bufferStructures);

            ~BatchRenderer2D() override;

            BatchRenderer2D(const BatchRenderer2D &) = delete;

            BatchRenderer2D &operator=(BatchRenderer2D &) = delete;

            std::vector<GLint> generateSamplerIDs();

        private:
            void _begin() override;

            void _submit(const components::Shape &position, const components::Appearance &color) override;

            void _submit(const components::Shape &position, const components::Texture &texture) override;

            void _submit(const components::Text &text) override;

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
            GLint getSamplerID(GLuint textureID);

        private:
            // Actual number of indices used.
            GLsizei mIndexCount;

            GLsizei mMaxSpriteCount;
            GLsizei mMaxVertexSize;
            const GLint mMaxNumTextureSamplers;
            GLsizei mMaxIndicesCount;
            GLsizei mMaxSpriteSize;
            GLsizei mMaxBufferSize;

            GLuint mVDO;

            std::unique_ptr<buffers::VertexArray> mVAO;
            std::unique_ptr<buffers::IndexBuffer> mIBO;

            GLint mNextSamplerID{0};
            std::vector<GLint> mSamplers{};
            std::vector<GLuint> mTextures{};

            // The buffer that will hold components::Vertex data, or its variants, in the batch.
            void *mBuffer;
        };

    }
}