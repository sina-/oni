#include <oni-core/graphic/oni-graphic-renderer-ogl-tessellation.h>

#include <numeric>
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
                Renderer_OpenGL(PrimitiveType::POINT),
                mMaxPrimitiveCount{maxSpriteCount} {
            mVertexSize = sizeof(graphic::TessellationVertex);
            mMaxIndicesCount = mMaxPrimitiveCount;
            mPrimitiveSize = mVertexSize * 1;

            assert(mMaxIndicesCount < std::numeric_limits<common::i32>::max());

            common::oniGLsizei maxBufferSize{mPrimitiveSize * mMaxPrimitiveCount};

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
            auto halfSizeIndex = glGetAttribLocation(program, "halfSize");
            auto colorIndex = glGetAttribLocation(program, "color");
            auto uvIndex = glGetAttribLocation(program, "uv");
            auto samplerIDIndex = glGetAttribLocation(program, "samplerID");

            if (positionIndex == -1 || headingIndex == -1 || uvIndex == -1 || colorIndex == -1 ||
                samplerIDIndex == -1 || halfSizeIndex == -1) {
                assert(false);
            }

            common::oniGLsizei stride = mVertexSize;

            graphic::BufferStructure sampler;
            sampler.index = static_cast<common::oniGLuint>(samplerIDIndex);
            sampler.componentCount = 1;
            sampler.componentType = GL_FLOAT;
            sampler.normalized = GL_FALSE;
            sampler.stride = stride;
            sampler.offset = static_cast<const common::oniGLvoid *>(nullptr);

            graphic::BufferStructure halfSize;
            halfSize.index = static_cast<common::oniGLuint>(halfSizeIndex);
            halfSize.componentCount = 2;
            halfSize.componentType = GL_FLOAT;
            halfSize.normalized = GL_FALSE;
            halfSize.stride = stride;
            halfSize.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(graphic::TessellationVertex,
                                                                                   halfSize));

            graphic::BufferStructure heading;
            heading.index = static_cast<common::oniGLuint>(headingIndex);
            heading.componentCount = 1;
            heading.componentType = GL_FLOAT;
            heading.normalized = GL_FALSE;
            heading.stride = stride;
            heading.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(graphic::TessellationVertex,
                                                                                  heading));

            graphic::BufferStructure position;
            position.index = static_cast<common::oniGLuint>(positionIndex);
            position.componentCount = 3;
            position.componentType = GL_FLOAT;
            position.normalized = GL_FALSE;
            position.stride = stride;
            position.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(graphic::TessellationVertex,
                                                                                   position));

            graphic::BufferStructure color;
            color.index = static_cast<common::oniGLuint>(colorIndex);
            color.componentCount = 4;
            color.componentType = GL_FLOAT;
            color.normalized = GL_TRUE;
            color.stride = stride;
            color.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(graphic::TessellationVertex, color));

            graphic::BufferStructure uv;
            uv.index = static_cast<common::oniGLuint>(uvIndex);
            uv.componentCount = 2;
            uv.componentType = GL_FLOAT;
            uv.normalized = GL_TRUE;
            uv.stride = stride;
            uv.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(graphic::TessellationVertex, uv));

            std::vector<graphic::BufferStructure> bufferStructures;
            bufferStructures.push_back(sampler);
            bufferStructures.push_back(halfSize);
            bufferStructures.push_back(heading);
            bufferStructures.push_back(position);
            bufferStructures.push_back(color);
            bufferStructures.push_back(uv);

            mVertexArray = std::make_unique<buffer::VertexArray>(bufferStructures, maxBufferSize);

            if (mMaxIndicesCount > mMaxPrimitiveCount) {
                mIndexBuffer = std::make_unique<buffer::IndexBuffer>(mMaxIndicesCount);
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
        Renderer_OpenGL_Tessellation::submit(const component::WorldP3D &pos,
                                             const component::Heading &heading,
                                             const component::Scale &scale,
                                             const component::Color &color,
                                             const component::Texture &texture) {
            assert(mIndexCount + 1 < mMaxIndicesCount);
            auto buffer = static_cast<graphic::TessellationVertex *>(mBuffer);

            common::i32 samplerID = -1;
            if (!texture.image.path.empty()) {
                samplerID = getSamplerID(texture.textureID);
            }

            buffer->position = pos.value;
            buffer->heading = heading.value;
            buffer->halfSize = math::vec2{scale.x / 2.f, scale.y / 2.f}; // TODO: Why not vec2 for Scale?
            buffer->color = color.rgba();
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

        // TODO: Move this into a font renderer
        void
        Renderer_OpenGL_Tessellation::submit(const component::Text &text,
                                             const component::WorldP3D &pos) {
            assert(false); // TODO: Need to re-implement this function
            auto buffer = static_cast<graphic::TessellationVertex *>(mBuffer);

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
        Renderer_OpenGL_Tessellation::enableShader(const math::mat4 &model,
                                                   const math::mat4 &view,
                                                   const math::mat4 &proj,
                                                   const math::vec2 &screenSize,
                                                   common::r32 zoom) {
            mShader->enable();
            mShader->setUniformMat4("view", view);
            mShader->setUniformMat4("proj", proj);
        }

        void
        Renderer_OpenGL_Tessellation::disableShader() {
            mShader->disable();
        }

        void
        Renderer_OpenGL_Tessellation::bindVertexArray() {
            mVertexArray->bindVAO();
        }

        void
        Renderer_OpenGL_Tessellation::unbindVertexArray() {
            mVertexArray->unbindVAO();
        }

        void
        Renderer_OpenGL_Tessellation::bindVertexBuffer() {
            mVertexArray->bindVBO();
        }

        void
        Renderer_OpenGL_Tessellation::unbindVertexBuffer() {
            mVertexArray->unbindVBO();
        }

        void
        Renderer_OpenGL_Tessellation::bindIndexBuffer() {
            if (mIndexBuffer) {
                mIndexBuffer->bind();
            }
        }

        void
        Renderer_OpenGL_Tessellation::unbindIndexBuffer() {
            if (mIndexBuffer) {
                mIndexBuffer->unbind();
            }
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