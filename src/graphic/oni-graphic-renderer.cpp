#include <oni-core/graphic/oni-graphic-renderer.h>

namespace oni {
    namespace graphic {
        Renderer::Renderer() = default;

        Renderer::~Renderer() = default;

        void
        Renderer::begin(const math::mat4 &model,
                          const math::mat4 &view,
                          const math::mat4 &proj,
                          const math::vec2 &screenSize,
                          common::r32 zoom) {
            assert(!mBegun);
            mBegun = true;
            _begin(model, view, proj, screenSize, zoom);
        }

        void
        Renderer::end() {
            assert(mBegun);
            _end();
            _flush();
            mBegun = false;
        }
    }
}