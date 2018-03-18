#pragma once

#include <map>
#include <stack>

// TODO: Don't include shit, forward declare them in the headers and include in CPP.
#include <ftgl/freetype-gl.h>
#include <ftgl/texture-atlas.h>

#include <buffers/index-buffer.h>
#include <buffers/vertex-array.h>
#include <components/visual.h>
#include <components/physical.h>
#include <graphics/renderer-2d.h>
#include <utils/oni-assert.h>
#include <graphics/font-manager.h>

namespace oni {
    namespace graphics {
        using namespace buffers;

        class BatchRenderer2D : public Renderer2D {

            unsigned long mMaxSpriteCount;
            unsigned long mMaxIndicesCount;

            unsigned long mMaxBufferSize;
            GLsizei mMaxVertexSize;
            GLsizei mMaxSpriteSize;

            std::unique_ptr<IndexBuffer> mIBO;

            // Actual number of indices used.
            GLsizei mIndexCount;

            GLuint mVDO;
            std::unique_ptr<VertexArray> mVAO;

            // The buffer that will hold components::Vertex data, or its variants, in the batch.
            void *mBuffer;

            // A mapping from texture id to 2D sampler that will draw it.
            std::map<GLuint, GLint> mTextureToSampler;
            std::vector<GLint> mSamplers;

            const unsigned long mMaxNumTextureSamplers;

        public:
            BatchRenderer2D(const unsigned long maxSpriteCount, unsigned long maxNumTextureSamplers,
                            GLsizei maxVertexSize,
                            components::BufferStructures bufferStructures);

            ~BatchRenderer2D() { glDeleteBuffers(1, &mVDO); };

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