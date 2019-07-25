#include <oni-core/graphic/oni-graphic-renderer-ogl-quad.h>

#include <GL/glew.h>

#include <oni-core/graphic/buffer/oni-graphic-buffer-data.h>
#include <oni-core/graphic/buffer/oni-graphic-index-buffer.h>
#include <oni-core/graphic/buffer/oni-graphic-frame-buffer.h>
#include <oni-core/graphic/buffer/oni-graphic-vertex-array.h>
#include <oni-core/graphic/oni-graphic-shader.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-visual.h>

namespace oni {
    namespace graphic {
        Renderer_OpenGL_Quad::Renderer_OpenGL_Quad(common::oniGLsizei maxPrimitiveCount) :
                Renderer_OpenGL(PrimitiveType::TRIANGLES), mMaxPrimitiveCount(maxPrimitiveCount) {
            mMaxIndicesCount = mMaxPrimitiveCount * 6;

            common::oniGLsizei vertexSize = sizeof(graphic::QuadVertex);
            common::oniGLsizei primitiveSize = vertexSize * 4;

            assert(mMaxIndicesCount < std::numeric_limits<common::i32>::max());

            common::oniGLsizei maxBufferSize{primitiveSize * mMaxPrimitiveCount};

            auto vertShader = std::string_view("resources/shaders/quad.vert");
            auto geomShader = std::string_view("");
            auto fragShader = std::string_view("resources/shaders/quad.frag");
            mShader = std::make_unique<graphic::Shader>(vertShader,
                                                        geomShader,
                                                        fragShader);

            auto program = mShader->getProgram();

            auto positionIndex = glGetAttribLocation(program, "position");
            auto colorIndex = glGetAttribLocation(program, "color");
            auto uvIndex = glGetAttribLocation(program, "uv");
            auto samplerFrontIndex = glGetAttribLocation(program, "samplerFront");
            auto samplerBackIndex = glGetAttribLocation(program, "samplerBack");

            if (positionIndex == -1 || uvIndex == -1 || colorIndex == -1 ||
                samplerFrontIndex == -1 || samplerBackIndex == -1) {
                assert(false);
            }

            graphic::BufferStructure samplerFront;
            samplerFront.index = static_cast<common::oniGLuint>(samplerFrontIndex);
            samplerFront.componentCount = 1;
            samplerFront.componentType = GL_FLOAT;
            samplerFront.normalized = GL_FALSE;
            samplerFront.stride = vertexSize;
            samplerFront.offset = static_cast<const common::oniGLvoid *>(nullptr);

            graphic::BufferStructure samplerBack;
            samplerBack.index = static_cast<common::oniGLuint>(samplerBackIndex);
            samplerBack.componentCount = 1;
            samplerBack.componentType = GL_FLOAT;
            samplerBack.normalized = GL_FALSE;
            samplerBack.stride = vertexSize;
            samplerBack.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(graphic::QuadVertex,
                                                                                      samplerBack));

            graphic::BufferStructure color;
            color.index = static_cast<common::oniGLuint>(colorIndex);
            color.componentCount = 4;
            color.componentType = GL_FLOAT;
            color.normalized = GL_TRUE;
            color.stride = vertexSize;
            color.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(graphic::QuadVertex, color));

            graphic::BufferStructure uv;
            uv.index = static_cast<common::oniGLuint>(uvIndex);
            uv.componentCount = 2;
            uv.componentType = GL_FLOAT;
            uv.normalized = GL_TRUE;
            uv.stride = vertexSize;
            uv.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(graphic::QuadVertex, uv));

            graphic::BufferStructure position;
            position.index = static_cast<common::oniGLuint>(positionIndex);
            position.componentCount = 3;
            position.componentType = GL_FLOAT;
            position.normalized = GL_FALSE;
            position.stride = vertexSize;
            position.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(graphic::QuadVertex, pos));

            std::vector<graphic::BufferStructure> bufferStructures;
            bufferStructures.push_back(samplerFront);
            bufferStructures.push_back(samplerBack);
            bufferStructures.push_back(color);
            bufferStructures.push_back(uv);
            bufferStructures.push_back(position);

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

        Renderer_OpenGL_Quad::~Renderer_OpenGL_Quad() = default;

        void
        Renderer_OpenGL_Quad::submit(const component::Quad &quad,
                                     const component::Color &color,
                                     const component::Texture *texture) {
            assert(mIndexCount + 6 < mMaxIndicesCount);

            auto buffer = static_cast<graphic::QuadVertex *>(mBuffer);

            auto samplerFront = -1.f;
            auto samplerBack = -1.f;
            auto uv0 = math::vec2{};
            auto uv1 = math::vec2{};
            auto uv2 = math::vec2{};
            auto uv3 = math::vec2{};
            if (texture) {
                samplerFront = getSamplerID(texture->textureID);
                uv0 = texture->uv[0];
                uv1 = texture->uv[1];
                uv2 = texture->uv[2];
                uv3 = texture->uv[3];
            }
            auto c = color.rgba();

            // a.
            buffer->pos = quad.a.value;
            buffer->color = c;
            buffer->uv = uv0;
            buffer->samplerFront = samplerFront;
            buffer->samplerBack = samplerBack;

            ++buffer;

            // b.
            buffer->pos = quad.b.value;
            buffer->color = c;
            buffer->uv = uv1;
            buffer->samplerFront = samplerFront;
            buffer->samplerBack = samplerBack;

            ++buffer;

            // c.
            buffer->pos = quad.c.value;
            buffer->color = c;
            buffer->uv = uv2;
            buffer->samplerFront = samplerFront;
            buffer->samplerBack = samplerBack;

            ++buffer;

            // d.
            buffer->pos = quad.d.value;
            buffer->color = c;
            buffer->uv = uv3;
            buffer->samplerFront = samplerFront;
            buffer->samplerBack = samplerBack;

            ++buffer;

            // Update the mBuffer to point to the head.
            mBuffer = static_cast<void *>(buffer);

            // +6 as there are 6 vertices that makes up two adjacent triangles but those triangles are
            // defined by 4 vertices only.
            /**
             *    1 +---+ 2
             *      |  /|
             *      |/  |
             *    0 +---+ 3
             **/
            mIndexCount += 6;
        }

        void
        Renderer_OpenGL_Quad::submit(const component::Quad &quad,
                                     const component::Color &color,
                                     const component::Texture &front,
                                     const component::Texture &back) {
            assert(mIndexCount + 6 < mMaxIndicesCount);

            assert(front.textureID);
            assert(back.textureID);

            assert(math::almost_Equal(front.uv[0].x, back.uv[0].x));
            assert(math::almost_Equal(front.uv[1].x, back.uv[1].x));
            assert(math::almost_Equal(front.uv[2].x, back.uv[2].x));
            assert(math::almost_Equal(front.uv[3].x, back.uv[3].x));

            assert(math::almost_Equal(front.uv[0].y, back.uv[0].y));
            assert(math::almost_Equal(front.uv[1].y, back.uv[1].y));
            assert(math::almost_Equal(front.uv[2].y, back.uv[2].y));
            assert(math::almost_Equal(front.uv[3].y, back.uv[3].y));

            auto buffer = static_cast<graphic::QuadVertex *>(mBuffer);

            auto samplerFront = getSamplerID(front.textureID);
            auto samplerBack = getSamplerID(back.textureID);

            auto c = color.rgba();

            buffer->pos = quad.a.value;
            buffer->color = c;
            buffer->uv = front.uv[0];
            buffer->samplerFront = samplerFront;
            buffer->samplerBack = samplerBack;

            ++buffer;

            buffer->pos = quad.b.value;
            buffer->color = c;
            buffer->uv = front.uv[1];
            buffer->samplerFront = samplerFront;
            buffer->samplerBack = samplerBack;

            ++buffer;

            buffer->pos = quad.c.value;
            buffer->color = c;
            buffer->uv = front.uv[2];
            buffer->samplerFront = samplerFront;
            buffer->samplerBack = samplerBack;

            ++buffer;

            buffer->pos = quad.d.value;
            buffer->color = c;
            buffer->uv = front.uv[3];
            buffer->samplerFront = samplerFront;
            buffer->samplerBack = samplerBack;

            ++buffer;

            mBuffer = static_cast<void *>(buffer);

            mIndexCount += 6;
        }

        void
        Renderer_OpenGL_Quad::submit(const Renderable &renderable) {
            // TODO: Implement
            assert(false);
        }

        void
        Renderer_OpenGL_Quad::enableShader(const math::mat4 &model,
                                           const math::mat4 &view,
                                           const math::mat4 &proj,
                                           const math::vec2 &screenSize,
                                           common::r32 zoom) {
            mShader->enable();
            mShader->setUniformMat4("model", model);
            mShader->setUniformMat4("view", view);
            mShader->setUniformMat4("proj", proj);
        }

        common::oniGLsizei
        Renderer_OpenGL_Quad::getIndexCount() {
            return mIndexCount;
        }

        void
        Renderer_OpenGL_Quad::resetIndexCount() {
            mIndexCount = 0;
        }
    }
}