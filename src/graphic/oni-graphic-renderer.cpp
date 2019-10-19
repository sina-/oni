#include <oni-core/graphic/oni-graphic-renderer.h>

#include <cassert>

#include <oni-core/component/oni-component-visual.h>
#include <oni-core/math/oni-math-function.h>


namespace oni {
    Renderer::Renderer(TextureManager &tm) : mTextureManager(tm) {}

    Renderer::~Renderer() = default;

    Renderer::BlendSpec
    Renderer::getBlendSpec(Material_Finish_Enum type) {
        auto result = BlendSpec{};
        switch (type) {
            case Material_Finish_Enum::SOLID:
            case Material_Finish_Enum::TRANSLUCENT: {
                result.src = BlendMode::ONE;
                result.dest = BlendMode::ONE_MINUS_SRC_ALPHA;
                break;
            }
            case Material_Finish_Enum::SHINNY: {
                result.src = BlendMode::ONE;
                result.dest = BlendMode::ONE;
                break;
            }
            case Material_Finish_Enum::LAST:
            default: {
                assert(false);
                break;
            }
        }
        return result;
    }

    Renderer::DepthSpec
    Renderer::getDepthSpec(Material_Finish_Enum type) {
        auto result = DepthSpec{};
        switch (type) {
            case Material_Finish_Enum::SOLID: {
                result.depthRead = true;
                result.depthWrite = true;
                break;
            }
                // TODO: Is this correct?
            case Material_Finish_Enum::TRANSLUCENT:
            case Material_Finish_Enum::SHINNY: {
                result.depthRead = true;
                result.depthWrite = false;
                break;
            }
            case Material_Finish_Enum::LAST:
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
        // NOTE: To keep rendering order stable over entities that have equal z values.
        if (almost_Equal(left.pos->z, right.pos->z)) {
            return left.id > right.id;
        }
        return left.pos->z > right.pos->z;
    }

    bool
    operator>(const Renderable &left,
              const Renderable &right) {
        if (almost_Equal(left.pos->z, right.pos->z)) {
            return left.id < right.id;
        }
        return left.pos->z < right.pos->z;
    }
}