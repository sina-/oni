#pragma once

#include <oni-core/graphic/oni-graphic-renderer-ogl.h>

namespace oni {
    class Shader;

    class Renderer_OpenGL_Quad : public Renderer_OpenGL {
    public:
        explicit Renderer_OpenGL_Quad(oniGLsizei maxSpriteCount,
                                      TextureManager &);

        ~Renderer_OpenGL_Quad() override;

        void
        submit(const Renderable &renderable) override;

        // TODO: These are only used to render to texture, so there is no need to unify it at the moment.
        void
        submit(const Quad &,
               const Color &,
               const Texture *);

        void
        submit(const Quad &,
               const Color &,
               const Texture &front,
               const Texture &back);

    protected:
        void
        enableShader(const RenderSpec &) override;

        oniGLsizei
        getIndexCount() override;

        void
        resetIndexCount() override;

    private:
        oniGLsizei mMaxPrimitiveCount{0};
        oniGLsizei mMaxIndicesCount{0};

        // Actual number of indices used.
        oniGLsizei mIndexCount{0};
    };
}
