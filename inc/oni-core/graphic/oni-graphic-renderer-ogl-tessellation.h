#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/graphic/oni-graphic-renderer-ogl.h>
#include <oni-core/component/oni-component-fwd.h>

namespace oni {
    class Shader;

    class Renderer_OpenGL_Tessellation : public Renderer_OpenGL {
    public:
        explicit Renderer_OpenGL_Tessellation(oniGLsizei maxSpriteCount);

        ~Renderer_OpenGL_Tessellation() override;

        void
        submit(const Renderable &) override;

    protected:
        oniGLsizei
        getIndexCount() override;

        void
        resetIndexCount() override;

        void
        enableShader(const RenderSpec &) override;

    private:
        oniGLsizei mMaxPrimitiveCount{0};
        oniGLsizei mMaxIndicesCount{0};

        // Actual number of indices used.
        oniGLsizei mIndexCount{0};
    };
}