#include <numeric>
#include <cassert>

#include <GL/glew.h>

#include <oni-core/graphic/oni-graphic-batch-renderer-2d.h>
#include <oni-core/graphic/oni-graphic-texture-manager.h>
#include <oni-core/graphic/buffer/oni-graphic-buffer.h>
#include <oni-core/graphic/buffer/oni-graphic-index-buffer.h>
#include <oni-core/graphic/buffer/oni-graphic-vertex-array.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-physics.h>

namespace oni {
    namespace graphic {
        BatchRenderer2D::BatchRenderer2D(const common::oniGLsizei maxSpriteCount,
                                         const common::oniGLint maxNumTextureSamplers,
                                         const common::oniGLsizei maxVertexSize,
                                         const std::vector<graphic::BufferStructure> &bufferStructure,
                                         PrimitiveType type)
                : mIndexCount{0},
                  mMaxPrimitiveCount{maxSpriteCount},
                  mMaxVertexSize{maxVertexSize},
                  mMaxNumTextureSamplers{maxNumTextureSamplers},
                  mPrimitiveType{type} {

            switch (type) {
                case PrimitiveType::POINT: {
                    mMaxIndicesCount = mMaxPrimitiveCount;
                    mMaxPrimitiveSize = mMaxVertexSize * 1;
                    break;
                }
                case PrimitiveType::LINE: {
                    mMaxIndicesCount = mMaxPrimitiveCount;
                    mMaxPrimitiveSize = mMaxVertexSize * 2;
                    break;
                }
                case PrimitiveType::TRIANGLE: {
                    // Each sprite has 6 indices.
                    mMaxIndicesCount = mMaxPrimitiveCount * 6;

                    // Each sprite has 4 vertices (6 in reality but 4 of them share the same data).
                    mMaxPrimitiveSize = mMaxVertexSize * 4;

                    break;
                }
                default:
                    assert(false);
            }

            auto maxUIntSize = std::numeric_limits<common::i32>::max();
            assert(mMaxIndicesCount < maxUIntSize);

            common::oniGLsizei maxBufferSize{mMaxPrimitiveSize * mMaxPrimitiveCount};
            mVertexArray = std::make_unique<buffer::VertexArray>(bufferStructure, maxBufferSize);

            if (mMaxIndicesCount > mMaxPrimitiveCount) {
                mIndexBuffer = std::make_unique<buffer::IndexBuffer>(mMaxIndicesCount);
            }

            mBuffer = nullptr;
        }

        BatchRenderer2D::~BatchRenderer2D() = default;

        void
        BatchRenderer2D::_begin() {
            mNextSamplerID = 0;
            mSamplers.clear();
            mTextures.clear();

            mVertexArray->bindVBO();
            // Data written to mBuffer has to match the structure of VBO.
            mBuffer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        }

        void
        BatchRenderer2D::_submit(const component::WorldP3D &pos,
                                 const component::Heading &heading,
                                 const component::Scale &scale,
                                 const component::Appearance &appearance,
                                 const component::Texture &texture) {
            assert(mIndexCount + 1 < mMaxIndicesCount);

            auto buffer = static_cast<graphic::Vertex *>(mBuffer);

            common::i32 samplerID = -1;
            if (texture.status == component::TextureStatus::READY) {
                samplerID = getSamplerID(texture.textureID);
            }

            buffer->position = pos.value;
            buffer->heading = heading.value;
            buffer->halfSize = math::vec2{scale.x / 2.f, scale.y / 2.f}; // TODO: Why not vec2 for Scale?
            buffer->color = appearance.color;
            buffer->uv[0] = texture.uv[0];
            buffer->uv[1] = texture.uv[1];
            buffer->uv[2] = texture.uv[2];
            buffer->uv[3] = texture.uv[3];
            buffer->samplerID = samplerID;
            buffer++;

            // Update the mBuffer to point to the head.
            mBuffer = static_cast<void *>(buffer);

            mIndexCount += 1;
        }

        void
        BatchRenderer2D::_submit(const component::Text &text,
                                 const component::WorldP3D &pos) {
            assert(false); // TODO: Need to re-implement this function
            auto buffer = static_cast<graphic::Vertex *>(mBuffer);

            auto samplerID = getSamplerID(text.textureID);

            auto advance = 0.0f;

            auto scaleX = text.xScaling;
            auto scaleY = text.yScaling;

            common::r32 z = 1.f;

            for (common::u32 i = 0; i < text.textContent.size(); i++) {
                assert(mIndexCount + 6 < mMaxIndicesCount);

                auto x0 = pos.x + text.offsetX[i] / scaleX + advance;
                auto y0 = pos.y + text.offsetY[i] / scaleY;
                auto x1 = x0 + text.width[i] / scaleX;
                auto y1 = y0 - text.height[i] / scaleY;

                auto u0 = text.uv[i].x;
                auto v0 = text.uv[i].y;
                auto u1 = text.uv[i].z;
                auto v1 = text.uv[i].w;

                buffer->position = math::vec3{x0, y0, z};
                buffer->uv[0] = math::vec2{u0, v0};
                buffer->samplerID = samplerID;
                buffer++;

                buffer->position = math::vec3{x0, y1, z};
                buffer->uv[1] = math::vec2{u0, v1};
                buffer->samplerID = samplerID;
                buffer++;

                buffer->position = math::vec3{x1, y1, z};
                buffer->uv[2] = math::vec2{u1, v1};
                buffer->samplerID = samplerID;
                buffer++;

                buffer->position = math::vec3{x1, y0, z};
                buffer->uv[3] = math::vec2{u1, v0};
                buffer->samplerID = samplerID;
                buffer++;

                advance += text.advanceX[i] / scaleX;
                mIndexCount += 6;
            }

            mBuffer = static_cast<void *>(buffer);
        }

        void
        BatchRenderer2D::_flush() {
            TextureManager::bindRange(0, mTextures);

            mVertexArray->bindVAO();
            if (mIndexBuffer) {
                mIndexBuffer->bind();
            }

            switch (mPrimitiveType) {
                case PrimitiveType::POINT: {
                    glDrawArrays(GL_POINTS, 0, mIndexCount);
                    break;
                }
                case PrimitiveType::LINE: {
                    glDrawArrays(GL_LINES, 0, mIndexCount);
                    break;
                }
                case PrimitiveType::TRIANGLE: {
                    glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, nullptr);
                    break;
                }
                default: {
                    assert(false);
                }
            };

            if (mIndexBuffer) {
                mIndexBuffer->unbind();
            }
            mVertexArray->unbindVAO();

            TextureManager::unbind();

            mIndexCount = 0;
        }

        void
        BatchRenderer2D::_end() {
            glUnmapBuffer(GL_ARRAY_BUFFER);
            mVertexArray->unbindVBO();
        }

        void
        BatchRenderer2D::reset() {
            end();
            flush();
            begin();
        }

        common::oniGLint
        BatchRenderer2D::getSamplerID(common::oniGLuint textureID) {
            auto it = std::find(mTextures.begin(), mTextures.end(), textureID);
            if (it == mTextures.end()) {
                /*
                 * This can be used to render more than max number of samplers, but it slows things down.
                if (mNextSamplerID > mMaxNumTextureSamplers) {
                    reset();
                }*/
                assert(mNextSamplerID <= mMaxNumTextureSamplers);

                mTextures.push_back(textureID);
                mSamplers.push_back(mNextSamplerID++);
                return mSamplers.back();
            } else {
                auto index = std::distance(mTextures.begin(), it);
                return mSamplers[index];
            }
        }

        std::vector<common::oniGLint>
        BatchRenderer2D::generateSamplerIDs() {
            std::vector<common::oniGLint> samplers(mMaxNumTextureSamplers, 0);
            // Fill the vector with 0, 1, 2, 3, ...
            std::iota(samplers.begin(), samplers.end(), 0);
            return samplers;
        }
    }
}
