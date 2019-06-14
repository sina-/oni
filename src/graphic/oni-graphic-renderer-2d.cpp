#include <oni-core/graphic/oni-graphic-renderer-2d.h>

namespace oni {
    namespace graphic {
        Renderer2D::Renderer2D() = default;

        Renderer2D::~Renderer2D() = default;

        void
        Renderer2D::begin(const math::mat4 &model,
                          const math::mat4 &view,
                          const math::mat4 &proj) {
            assert(!mBegun);
            mBegun = true;
            _begin(model, view, proj);
        }

        void
        Renderer2D::end() {
            assert(mBegun);
            _end();
            _flush();
            mBegun = false;
        }
    }
}