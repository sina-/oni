#include <oni-core/graphic/oni-graphic-renderer-ogl-strip.h>

#include <cassert>

#include <GL/glew.h>

#include <oni-core/graphic/buffer/oni-graphic-buffer.h>
#include <oni-core/graphic/buffer/oni-graphic-index-buffer.h>
#include <oni-core/graphic/buffer/oni-graphic-vertex-array.h>
#include <oni-core/graphic/buffer/oni-graphic-buffer-data.h>
#include <oni-core/graphic/oni-graphic-shader.h>
#include <oni-core/graphic/oni-graphic-texture-manager.h>


namespace oni {
    Renderer_OpenGL_Strip::Renderer_OpenGL_Strip(oniGLsizei maxSpriteCount,
                                                 TextureManager &tm) :
            Renderer_OpenGL(PrimitiveType::TRIANGLE_STRIP, tm),
            mMaxPrimitiveCount(maxSpriteCount) {
        mVertexSize = sizeof(StripVertex);
        mMaxIndicesCount = mMaxPrimitiveCount;
        mPrimitiveSize = mVertexSize * 1;

        assert(mMaxIndicesCount < std::numeric_limits<i32>::max());

        oniGLsizei maxBufferSize{mPrimitiveSize * mMaxPrimitiveCount};

        auto vertShader = std::string_view("oni-resources/shaders/strip.vert");
        auto geomShader = std::string_view("");
        auto fragShader = std::string_view("oni-resources/shaders/strip.frag");
        mShader = std::make_unique<Shader>(vertShader,
                                           geomShader,
                                           fragShader);

        auto program = mShader->getProgram();

        auto last_I = glGetAttribLocation(program, "last");
        auto current_I = glGetAttribLocation(program, "current");
        auto next_I = glGetAttribLocation(program, "next");
        auto bc_I = glGetAttribLocation(program, "bc");
        auto texoff_I = glGetAttribLocation(program, "texoff");

        auto color_I = 1;//glGetAttribLocation(program, "color");
        auto samplerID_I = 1;//glGetAttribLocation(program, "samplerID");

        if (last_I == -1 || current_I == -1 || next_I == -1 || texoff_I == -1 ||
            color_I == -1 || samplerID_I == -1 || bc_I == -1) {
            assert(false);
        }

        oniGLsizei stride = mVertexSize;

        BufferStructure last;
        last.index = static_cast<oniGLuint>(last_I);
        last.componentCount = 4;
        last.componentType = GL_FLOAT;
        last.normalized = GL_FALSE;
        last.stride = stride;
        last.offset = nullptr;

        BufferStructure current;
        current.index = static_cast<oniGLuint>(current_I);
        current.componentCount = 4;
        current.componentType = GL_FLOAT;
        current.normalized = GL_FALSE;
        current.stride = stride;
        current.offset = reinterpret_cast<const oniGLvoid *>(stride * 2);

        BufferStructure texoff;
        texoff.index = static_cast<oniGLuint>(texoff_I);
        texoff.componentCount = 1;
        texoff.componentType = GL_FLOAT;
        texoff.normalized = GL_FALSE;
        texoff.stride = stride;
        texoff.offset = reinterpret_cast<const oniGLvoid *>(stride * 2 +
                                                            offsetof(StripVertex, texoff));

        BufferStructure bc;
        bc.index = static_cast<oniGLuint>(bc_I);
        bc.componentCount = 3;
        bc.componentType = GL_FLOAT;
        bc.normalized = GL_FALSE;
        bc.stride = stride;
        bc.offset = reinterpret_cast<const oniGLvoid *>(stride * 2 + offsetof(StripVertex, bc));

        BufferStructure next;
        next.index = static_cast<oniGLuint>(next_I);
        next.componentCount = 4;
        next.componentType = GL_FLOAT;
        next.normalized = GL_FALSE;
        next.stride = stride;
        next.offset = reinterpret_cast<const oniGLvoid *>(stride * 4);

//            BufferStructure color;
//            color.index = static_cast<common::oniGLuint>(color_I);
//            color.componentCount = 4;
//            color.componentType = GL_FLOAT;
//            color.normalized = GL_TRUE;
//            color.stride = stride;
//            color.offset = reinterpret_cast<const common::oniGLvoid *>(stride * 2 + 4);
//
//            BufferStructure samplerID;
//            samplerID.index = static_cast<common::oniGLuint>(samplerID_I);
//            samplerID.componentCount = 1;
//            samplerID.componentType = GL_FLOAT;
//            samplerID.normalized = GL_TRUE;
//            samplerID.stride = stride;
//            samplerID.offset = reinterpret_cast<const common::oniGLvoid *>(stride * 2 + 4);

        std::vector<BufferStructure> bufferStructures;
        bufferStructures.push_back(last);
        bufferStructures.push_back(current);
        bufferStructures.push_back(texoff);
        bufferStructures.push_back(bc);
        bufferStructures.push_back(next);
        //bufferStructures.push_back(color);
        //bufferStructures.push_back(samplerID);

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

    Renderer_OpenGL_Strip::~Renderer_OpenGL_Strip() = default;

    void
    Renderer_OpenGL_Strip::submit(const WorldP3D &pos,
                                  const r32 offset,
                                  const Color &color,
                                  const Texture &texture) {
        assert(mIndexCount + 1 < mMaxIndicesCount);

        auto buffer = static_cast<StripVertex *>(mBuffer);

        i32 samplerID = -1;
        // TODO: This is not a good way to check for valid texture :/
        if (texture.image.width && texture.image.height) {
            samplerID = getSamplerID(texture.id);
        }

        buffer->center = vec4{pos.x, pos.y, pos.z, offset};
        buffer->texoff = color.a_r32();
        buffer->bc = color.rgb();
        ++buffer;

        mBuffer = static_cast<void *>(buffer);

        mIndexCount += 1;
    }

    void
    Renderer_OpenGL_Strip::enableShader(const RenderSpec &spec) {
        mShader->enable();
        mShader->setUniformMat4("view", spec.view);
        mShader->setUniformMat4("proj", spec.proj);
        mShader->setUniform2f("screenSize", spec.screenSize);
        mShader->setUniform1f("width", mStripeSize * spec.zoom);
    }

    oniGLsizei
    Renderer_OpenGL_Strip::getIndexCount() {
        // NOTE: Buffer structure uses last and next vertices to decide on the direction,
        // so I have to always draw 4 items, that is 4 * stride-- 1 * last + 1 * next + 1 * current + 1 * (texoff + bc)--, less
        // other-wise I will access out of bounds in the shader.
        return mIndexCount - 4;
    }

    void
    Renderer_OpenGL_Strip::resetIndexCount() {
        mIndexCount = 0;
    }

    void
    Renderer_OpenGL_Strip::setStripeSize(r32 size) {
        mStripeSize = size;
    }

    void
    Renderer_OpenGL_Strip::submit(const Renderable &renderable) {
        // TODO: I have to unify this class specific submit function with this one, merge the shaders used for this,
        // and then I can implement this function, unless I re-design my render pipeline to accomedate different
        // shaders better.
        assert(false);
    }
}