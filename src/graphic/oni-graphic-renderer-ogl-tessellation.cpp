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
    Renderer_OpenGL_Tessellation::Renderer_OpenGL_Tessellation(oniGLsizei maxSpriteCount) :
            Renderer_OpenGL(PrimitiveType::POINTS),
            mMaxPrimitiveCount{maxSpriteCount} {
        mMaxIndicesCount = mMaxPrimitiveCount;
        oniGLsizei vertexSize = sizeof(TessellationVertex);
        oniGLsizei primitiveSize = vertexSize * 1;

        assert(mMaxIndicesCount < std::numeric_limits<i32>::max());

        oniGLsizei maxBufferSize{primitiveSize * mMaxPrimitiveCount};

        auto vertShader = std::string_view("resources/shaders/tessellation.vert");
        auto geomShader = std::string_view("resources/shaders/tessellation.geom");
        auto fragShader = std::string_view("resources/shaders/tessellation.frag");
        // TODO: Resources are not part of oni-core library! This structure as is not flexible, meaning
        // I am forcing the users to only depend on built-in shaders. I should think of a better way
        // to provide flexibility in type of shaders users can define and expect to just work by having buffer
        // structure and what not set up automatically. Asset system should take care of this.
        // TODO: As it is now, BatchRenderer is coupled with the Shader. It requires the user to setup the
        // shader prior to render series of calls. Maybe shader should be built into the renderer.
        mShader = std::make_unique<Shader>(vertShader,
                                           geomShader,
                                           fragShader);

        auto program = mShader->getProgram();

        auto positionIndex = glGetAttribLocation(program, "position");
        auto orntIndex = glGetAttribLocation(program, "ornt");
        auto effectIndex = glGetAttribLocation(program, "effect");
        auto halfSizeIndex = glGetAttribLocation(program, "halfSize");
        auto colorIndex = glGetAttribLocation(program, "color");
        auto samplerIDIndex = glGetAttribLocation(program, "samplerID");
        auto uvIndex_0 = glGetAttribLocation(program, "uv_0");
        auto uvIndex_1 = glGetAttribLocation(program, "uv_1");
        auto uvIndex_2 = glGetAttribLocation(program, "uv_2");
        auto uvIndex_3 = glGetAttribLocation(program, "uv_3");

        if (positionIndex == -1 || orntIndex == -1 || colorIndex == -1 ||
            samplerIDIndex == -1 || halfSizeIndex == -1 ||
            effectIndex == -1 ||
            uvIndex_0 == -1 || uvIndex_1 == -1 || uvIndex_2 == -1 || uvIndex_3 == -1) {
            assert(false);
        }

        BufferStructure sampler;
        sampler.index = static_cast<oniGLuint>(samplerIDIndex);
        sampler.componentCount = 1;
        sampler.componentType = GL_FLOAT;
        sampler.normalized = GL_FALSE;
        sampler.stride = vertexSize;
        sampler.offset = static_cast<const oniGLvoid *>(nullptr);

        BufferStructure ornt;
        ornt.index = static_cast<oniGLuint>(orntIndex);
        ornt.componentCount = 1;
        ornt.componentType = GL_FLOAT;
        ornt.normalized = GL_FALSE;
        ornt.stride = vertexSize;
        ornt.offset = reinterpret_cast<const oniGLvoid *>(offsetof(TessellationVertex,
                                                                   ornt));

        BufferStructure effect;
        effect.index = static_cast<oniGLuint>(effectIndex);
        effect.componentCount = 1;
        effect.componentType = GL_FLOAT;
        effect.normalized = GL_FALSE;
        effect.stride = vertexSize;
        effect.offset = reinterpret_cast<const oniGLvoid *>(offsetof(TessellationVertex,
                                                                     effect));

        BufferStructure halfSize;
        halfSize.index = static_cast<oniGLuint>(halfSizeIndex);
        halfSize.componentCount = 2;
        halfSize.componentType = GL_FLOAT;
        halfSize.normalized = GL_FALSE;
        halfSize.stride = vertexSize;
        halfSize.offset = reinterpret_cast<const oniGLvoid *>(offsetof(TessellationVertex,
                                                                       halfSize));

        BufferStructure position;
        position.index = static_cast<oniGLuint>(positionIndex);
        position.componentCount = 3;
        position.componentType = GL_FLOAT;
        position.normalized = GL_FALSE;
        position.stride = vertexSize;
        position.offset = reinterpret_cast<const oniGLvoid *>(offsetof(TessellationVertex,
                                                                       position));

        BufferStructure color;
        color.index = static_cast<oniGLuint>(colorIndex);
        color.componentCount = 4;
        color.componentType = GL_FLOAT;
        color.normalized = GL_TRUE;
        color.stride = vertexSize;
        color.offset = reinterpret_cast<const oniGLvoid *>(offsetof(TessellationVertex, color));

        BufferStructure uv_0;
        uv_0.index = static_cast<oniGLuint>(uvIndex_0);
        uv_0.componentCount = 2;
        uv_0.componentType = GL_FLOAT;
        uv_0.normalized = GL_TRUE;
        uv_0.stride = vertexSize;
        uv_0.offset = reinterpret_cast<const oniGLvoid *>(offsetof(TessellationVertex, uv_0));

        BufferStructure uv_1;
        uv_1.index = static_cast<oniGLuint>(uvIndex_1);
        uv_1.componentCount = 2;
        uv_1.componentType = GL_FLOAT;
        uv_1.normalized = GL_TRUE;
        uv_1.stride = vertexSize;
        uv_1.offset = reinterpret_cast<const oniGLvoid *>(offsetof(TessellationVertex, uv_1));

        BufferStructure uv_2;
        uv_2.index = static_cast<oniGLuint>(uvIndex_2);
        uv_2.componentCount = 2;
        uv_2.componentType = GL_FLOAT;
        uv_2.normalized = GL_TRUE;
        uv_2.stride = vertexSize;
        uv_2.offset = reinterpret_cast<const oniGLvoid *>(offsetof(TessellationVertex, uv_2));

        BufferStructure uv_3;
        uv_3.index = static_cast<oniGLuint>(uvIndex_3);
        uv_3.componentCount = 2;
        uv_3.componentType = GL_FLOAT;
        uv_3.normalized = GL_TRUE;
        uv_3.stride = vertexSize;
        uv_3.offset = reinterpret_cast<const oniGLvoid *>(offsetof(TessellationVertex, uv_3));

        std::vector<BufferStructure> bufferStructures;
        bufferStructures.push_back(sampler);
        bufferStructures.push_back(ornt);
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
        for (i8 i = 0; i < mMaxNumTextureSamplers; ++i) {
            samplers.push_back(i);
        }

        mShader->enable();
        mShader->setUniformiv("samplers", samplers);
        mShader->disable();
    }

    Renderer_OpenGL_Tessellation::~Renderer_OpenGL_Tessellation() = default;

    void
    Renderer_OpenGL_Tessellation::submit(const Renderable &renderable) {
        // TODO: Break up the renderable into smaller pieces, with geometry being mandetory and the rest as pointers.
        // Just as they are in the registry. submit can split it down to more calls given other params
        assert(mIndexCount + 1 < mMaxIndicesCount);

        auto uv0 = vec2{};
        auto uv1 = vec2{};
        auto uv2 = vec2{};
        auto uv3 = vec2{};
        auto samplerID = -1;
        // TODO: would be nice to have an enum specific to the graphics that captures this?
        // Actually I CAN NOT enumCast MaterialTransition_Type yet :/ as None is still
        auto effectID = 0.f;
        auto color = Color{};

        const auto *skin = renderable.skin;
        const auto *text = renderable.text;
        const auto *animation = renderable.transitionAnimation;

        auto *buffer = static_cast<TessellationVertex *>(mBuffer);

        if (text) {
            // TODO: Add support for transitions! I could generalize MaterialSkin rendering to include text rendering.
            // Text is just an array of MaterialSkin where offsets and advance are used to calculate position
            // and uv, which I can do in a separate function and then I can unify this if block with else and support
            // transitions
            assert(!animation);
            assert(!skin);
            auto advance = 0.0f;

            auto scaleX = text->xScaling;
            auto scaleY = text->yScaling;

            effectID = 1.f;
            samplerID = getSamplerID(text->textureID);

            for (size i = 0; i < text->textContent.size(); i++) {
                auto x0 = text->offsetX[i] / scaleX + advance;
                auto x1 = x0 + text->width[i] / scaleX;

                auto y0 = text->offsetY[i] / scaleY;
                auto y1 = y0 - text->height[i] / scaleY;

                auto u0 = text->uv[i].x;
                auto v0 = text->uv[i].y;
                auto u1 = text->uv[i].z;
                auto v1 = text->uv[i].w;

                advance += text->advanceX[i] / scaleX;

                auto halfGlyphX = (x0 + x1) / 2.f;
                auto halfGlyphY = (y0 + y1) / 2.f;

                buffer->position = renderable.pos->value;
                buffer->position.x += halfGlyphX;
                buffer->position.y += halfGlyphY;

                buffer->ornt = renderable.ornt->value;
                buffer->effect = effectID;
                buffer->halfSize = vec2{halfGlyphX, halfGlyphY};
                buffer->color = {};
                buffer->uv_0 = vec2{u0, v0};
                buffer->uv_1 = vec2{u0, v1};
                buffer->uv_2 = vec2{u1, v1};
                buffer->uv_3 = vec2{u1, v0};
                buffer->samplerID = samplerID;

                ++buffer;
                mIndexCount += 1;
            }
        } else {
            if (skin && skin->texture.id) {
                assert(!animation);
                assert(!text);
                effectID = 1.f;
                uv0 = skin->texture.uv.values[0];
                uv1 = skin->texture.uv.values[1];
                uv2 = skin->texture.uv.values[2];
                uv3 = skin->texture.uv.values[3];
                samplerID = getSamplerID(skin->texture.id);
                color = skin->color;
            }

            const auto transitionType = renderable.def.transition;
            switch (transitionType) {
                case MaterialTransition_Type::NONE: {
                    break;
                }
                case MaterialTransition_Type::TEXTURE: {
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
                    } else {
                        assert(false);
                    }
                    break;
                }
                case MaterialTransition_Type::FADE: {
                    effectID = 2.f;
                    break;
                }
                case MaterialTransition_Type::TINT: {
                    // TODO: Very accurate and slow calculations, I don't need the accuracy but it can be faster!
                    auto &begin = renderable.transitionTint->begin;
                    auto &end = renderable.transitionTint->end;
                    // TODO: This is buggy, given blend function of (GL_ONE, GL_ONE) I can't produce black instead
                    // when rgb = 0 I just get transparent objects.
                    auto t = renderable.age->currentAge / renderable.age->maxAge;
                    auto r = lerp(begin.r_r32(), end.r_r32(), t);
                    auto g = lerp(begin.g_r32(), end.g_r32(), t);
                    auto b = lerp(begin.b_r32(), end.b_r32(), t);
                    color.set_r(r);
                    color.set_g(g);
                    color.set_b(b);
                    effectID = 3.f;
                    break;
                }
                default: {
                    assert(false);
                    break;
                }
            }

            buffer->position = renderable.pos->value;
            buffer->ornt = renderable.ornt->value;
            buffer->effect = effectID;
            buffer->halfSize = vec2{renderable.scale->x / 2.f,
                                    renderable.scale->y / 2.f};
            buffer->color = color.rgba();
            buffer->uv_0 = uv0;
            buffer->uv_1 = uv1;
            buffer->uv_2 = uv2;
            buffer->uv_3 = uv3;
            buffer->samplerID = samplerID;
            ++buffer;
            mIndexCount += 1;
        }

        // Update the mBuffer to point to the head.
        mBuffer = static_cast<void *>(buffer);
    }

    void
    Renderer_OpenGL_Tessellation::enableShader(const RenderSpec &spec) {
        mShader->enable();
        mShader->setUniformMat4("view", spec.view);
        mShader->setUniformMat4("proj", spec.proj);
    }

    oniGLsizei
    Renderer_OpenGL_Tessellation::getIndexCount() {
        return mIndexCount;
    }

    void
    Renderer_OpenGL_Tessellation::resetIndexCount() {
        mIndexCount = 0;
    }
}
