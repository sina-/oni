#pragma once

#include <oni-core/graphic/oni-graphic-renderer-ogl.h>
#include <oni-core/component/oni-component-geometry.h>


namespace oni {
    class Shader;

    class Renderer_OpenGL_Strip : public Renderer_OpenGL {
    public:
        explicit Renderer_OpenGL_Strip(oniGLsizei maxSpriteCount,
                                       TextureManager &);

        ~Renderer_OpenGL_Strip() override;

        void
        submit(const Renderable &renderable) override;

        void
        submit(const WorldP3D &,
               const r32 offset,
               const Color &color,
               const Texture &texture);

        void
        setStripeSize(r32);

    protected:
        void
        enableShader(const RenderSpec &) override;

        oniGLsizei
        getIndexCount() override;

        void
        resetIndexCount() override;

    private:
        oniGLsizei mMaxPrimitiveCount{0};
        oniGLsizei mVertexSize{0};
        oniGLsizei mMaxIndicesCount{0};
        oniGLsizei mPrimitiveSize{0};

        oniGLsizei mIndexCount{0};

        r32 mStripeSize{0.4f};
    };
}

