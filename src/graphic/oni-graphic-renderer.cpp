#include <oni-core/graphic/oni-graphic-renderer.h>
#include <oni-core/component/oni-component-visual.h>

namespace oni {
    namespace graphic {
        Renderer::Renderer() = default;

        Renderer::~Renderer() = default;

        void
        Renderer::begin(const math::mat4 &model,
                        const math::mat4 &view,
                        const math::mat4 &proj,
                        const math::vec2 &screenSize,
                        common::r32 zoom,
                        component::Texture *renderTarget) {
            assert(!mBegun);
            mBegun = true;

            if (renderTarget) {
                mRenderTarget = renderTarget;
                mViewportSize = getViewportSize();
                setViewportSize({renderTarget->image.width, renderTarget->image.height});
            }

            _begin(model, view, proj, screenSize, zoom);
        }

        void
        Renderer::end() {
            assert(mBegun);
            _end();
            _flush(mRenderTarget);

            if (mRenderTarget) {
                setViewportSize(mViewportSize);
            }

            mBegun = false;
        }
    }
}