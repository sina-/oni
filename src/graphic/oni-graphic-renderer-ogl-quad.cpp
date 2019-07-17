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
            auto samplerIDIndex = glGetAttribLocation(program, "samplerID");

            if (positionIndex == -1 || uvIndex == -1 || colorIndex == -1 ||
                samplerIDIndex == -1) {
                assert(false);
            }

            graphic::BufferStructure sampler;
            sampler.index = static_cast<common::oniGLuint>(samplerIDIndex);
            sampler.componentCount = 1;
            sampler.componentType = GL_FLOAT;
            sampler.normalized = GL_FALSE;
            sampler.stride = vertexSize;
            sampler.offset = static_cast<const common::oniGLvoid *>(nullptr);

            graphic::BufferStructure position;
            position.index = static_cast<common::oniGLuint>(positionIndex);
            position.componentCount = 3;
            position.componentType = GL_FLOAT;
            position.normalized = GL_FALSE;
            position.stride = vertexSize;
            position.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(graphic::QuadVertex, pos));

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

            std::vector<graphic::BufferStructure> bufferStructures;
            bufferStructures.push_back(sampler);
            bufferStructures.push_back(position);
            bufferStructures.push_back(color);
            bufferStructures.push_back(uv);

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
        Renderer_OpenGL_Quad::submit(const component::WorldP3D *pos,
                                     const component::Color &color,
                                     const component::Texture &texture) {
            assert(mIndexCount + 6 < mMaxIndicesCount);

            auto buffer = static_cast<graphic::QuadVertex *>(mBuffer);

            common::i32 samplerID = -1;
            if (!texture.image.path.empty()) {
                samplerID = getSamplerID(texture.textureID);
            }
            auto c = color.rgba();

            // a.
            buffer->pos = pos->value;
            buffer->color = c;
            buffer->uv = texture.uv[0];
            buffer->samplerID = samplerID;

            ++buffer;
            ++pos;

            // b.
            buffer->pos = pos->value;
            buffer->color = c;
            buffer->uv = texture.uv[1];
            buffer->samplerID = samplerID;

            ++buffer;
            ++pos;

            // c.
            buffer->pos = pos->value;
            buffer->color = c;
            buffer->uv = texture.uv[2];
            buffer->samplerID = samplerID;

            ++buffer;
            ++pos;

            // d.
            buffer->pos = pos->value;
            buffer->color = c;
            buffer->uv = texture.uv[3];
            buffer->samplerID = samplerID;

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
        Renderer_OpenGL_Quad::enableShader(const math::mat4 &model,
                                           const math::mat4 &view,
                                           const math::mat4 &proj,
                                           const math::vec2 &screenSize,
                                           common::r32 zoom) {
            mShader->enable();
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