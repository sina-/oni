#include <oni-core/graphic/oni-graphic-renderer-ogl-tessellation.h>

#include <cassert>

#include <GL/glew.h>

#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/graphic/buffer/oni-graphic-buffer.h>
#include <oni-core/graphic/buffer/oni-graphic-index-buffer.h>
#include <oni-core/graphic/buffer/oni-graphic-vertex-array.h>
#include <oni-core/graphic/buffer/oni-graphic-buffer-data.h>
#include <oni-core/graphic/oni-graphic-shader.h>
#include <oni-core/graphic/oni-graphic-texture-manager.h>


namespace oni {
    namespace graphic {
        Renderer_OpenGL_Tessellation::Renderer_OpenGL_Tessellation(common::oniGLsizei maxSpriteCount) :
                Renderer_OpenGL(PrimitiveType::POINTS),
                mMaxPrimitiveCount{maxSpriteCount} {
            mMaxIndicesCount = mMaxPrimitiveCount;
            common::oniGLsizei vertexSize = sizeof(graphic::TessellationVertex);
            common::oniGLsizei primitiveSize = vertexSize * 1;

            assert(mMaxIndicesCount < std::numeric_limits<common::i32>::max());

            common::oniGLsizei maxBufferSize{primitiveSize * mMaxPrimitiveCount};

            auto vertShader = std::string_view("resources/shaders/tessellation.vert");
            auto geomShader = std::string_view("resources/shaders/tessellation.geom");
            auto fragShader = std::string_view("resources/shaders/tessellation.frag");
            // TODO: Resources are not part of oni-core library! This structure as is not flexible, meaning
            // I am forcing the users to only depend on built-in shaders. I should think of a better way
            // to provide flexibility in type of shaders users can define and expect to just work by having buffer
            // structure and what not set up automatically. Asset system should take care of this.
            // TODO: As it is now, BatchRenderer is coupled with the Shader. It requires the user to setup the
            // shader prior to render series of calls. Maybe shader should be built into the renderer.
            mShader = std::make_unique<graphic::Shader>(vertShader,
                                                        geomShader,
                                                        fragShader);

            auto program = mShader->getProgram();

            auto positionIndex = glGetAttribLocation(program, "position");
            auto headingIndex = glGetAttribLocation(program, "heading");
            auto effectIndex = glGetAttribLocation(program, "effect");
            auto halfSizeIndex = glGetAttribLocation(program, "halfSize");
            auto colorIndex = glGetAttribLocation(program, "color");
            auto samplerIDIndex = glGetAttribLocation(program, "samplerID");
            auto uvIndex_0 = glGetAttribLocation(program, "uv_0");
            auto uvIndex_1 = glGetAttribLocation(program, "uv_1");
            auto uvIndex_2 = glGetAttribLocation(program, "uv_2");
            auto uvIndex_3 = glGetAttribLocation(program, "uv_3");

            if (positionIndex == -1 || headingIndex == -1 || colorIndex == -1 ||
                samplerIDIndex == -1 || halfSizeIndex == -1 ||
                effectIndex == -1 ||
                uvIndex_0 == -1 || uvIndex_1 == -1 || uvIndex_2 == -1 || uvIndex_3 == -1) {
                assert(false);
            }

            graphic::BufferStructure sampler;
            sampler.index = static_cast<common::oniGLuint>(samplerIDIndex);
            sampler.componentCount = 1;
            sampler.componentType = GL_FLOAT;
            sampler.normalized = GL_FALSE;
            sampler.stride = vertexSize;
            sampler.offset = static_cast<const common::oniGLvoid *>(nullptr);

            graphic::BufferStructure heading;
            heading.index = static_cast<common::oniGLuint>(headingIndex);
            heading.componentCount = 1;
            heading.componentType = GL_FLOAT;
            heading.normalized = GL_FALSE;
            heading.stride = vertexSize;
            heading.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(graphic::TessellationVertex,
                                                                                  heading));

            graphic::BufferStructure effect;
            effect.index = static_cast<common::oniGLuint>(effectIndex);
            effect.componentCount = 1;
            effect.componentType = GL_FLOAT;
            effect.normalized = GL_FALSE;
            effect.stride = vertexSize;
            effect.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(graphic::TessellationVertex,
                                                                                 effect));

            graphic::BufferStructure halfSize;
            halfSize.index = static_cast<common::oniGLuint>(halfSizeIndex);
            halfSize.componentCount = 2;
            halfSize.componentType = GL_FLOAT;
            halfSize.normalized = GL_FALSE;
            halfSize.stride = vertexSize;
            halfSize.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(graphic::TessellationVertex,
                                                                                   halfSize));

            graphic::BufferStructure position;
            position.index = static_cast<common::oniGLuint>(positionIndex);
            position.componentCount = 3;
            position.componentType = GL_FLOAT;
            position.normalized = GL_FALSE;
            position.stride = vertexSize;
            position.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(graphic::TessellationVertex,
                                                                                   position));

            graphic::BufferStructure color;
            color.index = static_cast<common::oniGLuint>(colorIndex);
            color.componentCount = 4;
            color.componentType = GL_FLOAT;
            color.normalized = GL_TRUE;
            color.stride = vertexSize;
            color.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(graphic::TessellationVertex, color));

            graphic::BufferStructure uv_0;
            uv_0.index = static_cast<common::oniGLuint>(uvIndex_0);
            uv_0.componentCount = 2;
            uv_0.componentType = GL_FLOAT;
            uv_0.normalized = GL_TRUE;
            uv_0.stride = vertexSize;
            uv_0.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(graphic::TessellationVertex, uv_0));

            graphic::BufferStructure uv_1;
            uv_1.index = static_cast<common::oniGLuint>(uvIndex_1);
            uv_1.componentCount = 2;
            uv_1.componentType = GL_FLOAT;
            uv_1.normalized = GL_TRUE;
            uv_1.stride = vertexSize;
            uv_1.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(graphic::TessellationVertex, uv_1));

            graphic::BufferStructure uv_2;
            uv_2.index = static_cast<common::oniGLuint>(uvIndex_2);
            uv_2.componentCount = 2;
            uv_2.componentType = GL_FLOAT;
            uv_2.normalized = GL_TRUE;
            uv_2.stride = vertexSize;
            uv_2.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(graphic::TessellationVertex, uv_2));

            graphic::BufferStructure uv_3;
            uv_3.index = static_cast<common::oniGLuint>(uvIndex_3);
            uv_3.componentCount = 2;
            uv_3.componentType = GL_FLOAT;
            uv_3.normalized = GL_TRUE;
            uv_3.stride = vertexSize;
            uv_3.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(graphic::TessellationVertex, uv_3));

            std::vector<graphic::BufferStructure> bufferStructures;
            bufferStructures.push_back(sampler);
            bufferStructures.push_back(heading);
            bufferStructures.push_back(effect);
            bufferStructures.push_back(halfSize);
            bufferStructures.push_back(position);
            bufferStructures.push_back(color);
            bufferStructures.push_back(uv_0);
            bufferStructures.push_back(uv_1);
            bufferStructures.push_back(uv_2);
            bufferStructures.push_back(uv_3);

            mVertexArray = std::make_unique<VertexArray>(bufferStructures, maxBufferSize);

            if (mMaxIndicesCount > mMaxPrimitiveCount) {
                mIndexBuffer = std::make_unique<IndexBuffer>(mMaxIndicesCount);
            }

            std::vector<GLint> samplers;
            for (common::i8 i = 0; i < mMaxNumTextureSamplers; ++i) {
                samplers.push_back(i);
            }

            mShader->enable();
            mShader->setUniformiv("samplers", samplers);
            mShader->disable();
        }

        Renderer_OpenGL_Tessellation::~Renderer_OpenGL_Tessellation() = default;

        void
        Renderer_OpenGL_Tessellation::submit(const Renderable &renderable) {
            assert(mIndexCount + 1 < mMaxIndicesCount);

            auto uv0 = math::vec2{};
            auto uv1 = math::vec2{};
            auto uv2 = math::vec2{};
            auto uv3 = math::vec2{};
            auto samplerID = -1;
            auto effectID = 0.f;

            const auto *skin = renderable.skin;
            const auto *animation = renderable.transitionAnimation;
            const auto transitionType = renderable.def.transition;

            if (skin && skin->texture.id) {
                effectID = 1.f;
                uv0 = skin->texture.uv.values[0];
                uv1 = skin->texture.uv.values[1];
                uv2 = skin->texture.uv.values[2];
                uv3 = skin->texture.uv.values[3];
                samplerID = getSamplerID(skin->texture.id);
                assert(!animation);
            }

            switch (transitionType) {
                case component::MaterialTransition_Type::NONE: {
                    break;
                }
                case component::MaterialTransition_Type::FADE: {
                    effectID = 2.f;
                    break;
                }
                case component::MaterialTransition_Type::TINT: {
                    effectID = 3.f;
                    break;
                }
                case component::MaterialTransition_Type::ANIMATED: {
                    if (animation) {
                        effectID = 1.f;
                        auto currentFrame = animation->value.nextFrame;
                        if (currentFrame < animation->value.frameUV.size()) {
                            uv0 = animation->value.frameUV[currentFrame].values[0];
                            uv1 = animation->value.frameUV[currentFrame].values[1];
                            uv2 = animation->value.frameUV[currentFrame].values[2];
                            uv3 = animation->value.frameUV[currentFrame].values[3];
                            samplerID = getSamplerID(animation->value.texture.id);
                        } else {
                            assert(false);
                        }
                        assert(!skin);
                        break;
                    } else {
                        assert(false);
                    }
                }
                case component::MaterialTransition_Type::LAST:
                default: {
                    assert(false);
                    break;
                }
            }

            auto *buffer = static_cast<graphic::TessellationVertex *>(mBuffer);

            buffer->position = renderable.pos->value;
            buffer->heading = renderable.heading->value;
            buffer->effect = effectID;
            buffer->halfSize = math::vec2{renderable.scale->x / 2.f,
                                          renderable.scale->y / 2.f}; // TODO: Why not vec2 for Scale?
            if (renderable.skin) {
                buffer->color = renderable.skin->color.rgba();
            } else {
                buffer->color = {};
            }
            buffer->uv_0 = uv0;
            buffer->uv_1 = uv1;
            buffer->uv_2 = uv2;
            buffer->uv_3 = uv3;
            buffer->samplerID = samplerID;
            ++buffer;

            // Update the mBuffer to point to the head.
            mBuffer = static_cast<void *>(buffer);

            mIndexCount += 1;
        }

        // TODO: Move this into a font renderer
        void
        Renderer_OpenGL_Tessellation::submit(const component::Text &text,
                                             const component::WorldP3D &pos) {
            assert(false); // TODO: Need to re-implement this function
            auto *buffer = static_cast<graphic::TessellationVertex *>(mBuffer);

            auto samplerID = getSamplerID(text.textureID);

            auto advance = 0.0f;

            auto scaleX = text.xScaling;
            auto scaleY = text.yScaling;

            common::r32 z = 1.f;

            for (common::size i = 0; i < text.textContent.size(); i++) {
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
                buffer->uv_0 = math::vec2{u0, v0};
                buffer->samplerID = samplerID;
                ++buffer;

                buffer->position = math::vec3{x0, y1, z};
                buffer->uv_1 = math::vec2{u0, v1};
                buffer->samplerID = samplerID;
                ++buffer;

                buffer->position = math::vec3{x1, y1, z};
                buffer->uv_2 = math::vec2{u1, v1};
                buffer->samplerID = samplerID;
                ++buffer;

                buffer->position = math::vec3{x1, y0, z};
                buffer->uv_3 = math::vec2{u1, v0};
                buffer->samplerID = samplerID;
                ++buffer;

                advance += text.advanceX[i] / scaleX;
                mIndexCount += 6;
            }

            mBuffer = static_cast<void *>(buffer);
        }

        void
        Renderer_OpenGL_Tessellation::enableShader(const RenderSpec &spec) {
            mShader->enable();
            mShader->setUniformMat4("view", spec.view);
            mShader->setUniformMat4("proj", spec.proj);
        }

        common::oniGLsizei
        Renderer_OpenGL_Tessellation::getIndexCount() {
            return mIndexCount;
        }

        void
        Renderer_OpenGL_Tessellation::resetIndexCount() {
            mIndexCount = 0;
        }
    }
}
