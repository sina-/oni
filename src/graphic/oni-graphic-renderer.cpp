#include <oni-core/graphic/oni-graphic-renderer.h>

#include <oni-core/component/oni-component-visual.h>


namespace oni {
    namespace graphic {
        Renderer::Renderer() = default;

        Renderer::~Renderer() = default;

        Renderer::BlendSpec
        Renderer::getBlendSpec(component::MaterialFinishType type) {
            auto result = BlendSpec{};
            switch (type) {
                case component::MaterialFinishType::SOLID:
                case component::MaterialFinishType::TRANSLUCENT: {
                    result.src = BlendMode::ONE;
                    result.dest = BlendMode::ONE_MINUS_SRC_ALPHA;
                    break;
                }
                case component::MaterialFinishType::SHINNY: {
                    result.src = BlendMode::ONE;
                    result.dest = BlendMode::ONE;
                    break;
                }
                case component::MaterialFinishType::LAST:
                default: {
                    assert(false);
                    break;
                }
            }
            return result;
        }

        Renderer::DepthSpec
        Renderer::getDepthSpec(component::MaterialFinishType type) {
            auto result = DepthSpec{};
            switch (type) {
                case component::MaterialFinishType::SOLID: {
                    result.depthRead = true;
                    result.depthWrite = true;
                    break;
                }
                    // TODO: Is this correct?
                case component::MaterialFinishType::TRANSLUCENT:
                case component::MaterialFinishType::SHINNY: {
                    result.depthRead = true;
                    result.depthWrite = false;
                    break;
                }
                case component::MaterialFinishType::LAST:
                default: {
                    assert(false);
                    break;
                }
            }
            return result;
        }

        void
        Renderer::begin(const RenderSpec &renderSpec) {
            assert(!mBegun);
            mBegun = true;

            if (renderSpec.renderTarget) {
                mRenderTarget = renderSpec.renderTarget;
                mViewportSize = getViewportSize();
                setViewportSize({renderSpec.renderTarget->image.width, renderSpec.renderTarget->image.height});
            }

            auto blendSpec = getBlendSpec(renderSpec.finishType);
            auto depthSpec = getDepthSpec(renderSpec.finishType);
            _begin(renderSpec, blendSpec, depthSpec);
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

        bool
        operator<(const Renderable &left,
                  const Renderable &right) {
            return left.pos->z >= right.pos->z;
        }

        bool
        operator>(const Renderable &left,
                  const Renderable &right) {
            return left.pos->z <= right.pos->z;
        }
    }
}