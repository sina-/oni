#include <oni-core/graphic/oni-graphic-renderer.h>

#include <oni-core/component/oni-component-visual.h>


namespace oni {
    Renderer::Renderer(TextureManager &tm) : mTextureManager(tm) {}

    Renderer::~Renderer() = default;

    Renderer::BlendSpec
    Renderer::getBlendSpec(MaterialFinish_Type type) {
        auto result = BlendSpec{};
        switch (type) {
            case MaterialFinish_Type::SOLID:
            case MaterialFinish_Type::TRANSLUCENT: {
                result.src = BlendMode::ONE;
                result.dest = BlendMode::ONE_MINUS_SRC_ALPHA;
                break;
            }
            case MaterialFinish_Type::SHINNY: {
                result.src = BlendMode::ONE;
                result.dest = BlendMode::ONE;
                break;
            }
            case MaterialFinish_Type::TRANSLUCENT_AND_SHINNY: {
                result.src = BlendMode::ONE;
                result.dest = BlendMode::ONE_MINUS_SRC_ALPHA;
                break;
            }
            case MaterialFinish_Type::LAST:
            default: {
                assert(false);
                break;
            }
        }
        return result;
    }

    Renderer::DepthSpec
    Renderer::getDepthSpec(MaterialFinish_Type type) {
        auto result = DepthSpec{};
        switch (type) {
            case MaterialFinish_Type::SOLID: {
                result.depthRead = true;
                result.depthWrite = true;
                break;
            }
                // TODO: Is this correct?
            case MaterialFinish_Type::TRANSLUCENT:
            case MaterialFinish_Type::TRANSLUCENT_AND_SHINNY :
            case MaterialFinish_Type::SHINNY: {
                result.depthRead = true;
                result.depthWrite = false;
                break;
            }
            case MaterialFinish_Type::LAST:
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