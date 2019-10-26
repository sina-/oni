#include <oni-core/graphic/oni-graphic-renderer.h>

#include <cassert>

#include <oni-core/component/oni-component-visual.h>
#include <oni-core/math/oni-math-function.h>


namespace oni {
    Renderer::Renderer(TextureManager &tm) : mTextureManager(tm) {}

    Renderer::~Renderer() = default;

    Renderer::BlendSpec
    Renderer::getBlendSpec(Material_Finish type) {
        auto result = BlendSpec{};
        if (type == Material_Finish::get("solid") ||
            type == Material_Finish::get("translucent")) {
            result.src = BlendMode::ONE;
            result.dest = BlendMode::ONE_MINUS_SRC_ALPHA;
        } else if (type == Material_Finish::get("shinny")) {
            result.src = BlendMode::ONE;
            result.dest = BlendMode::ONE;
        } else {
            assert(false);
            result.src = BlendMode::ONE;
            result.dest = BlendMode::ONE;
        }
        return result;
    }

    Renderer::DepthSpec
    Renderer::getDepthSpec(Material_Finish type) {
        auto result = DepthSpec{};
        if (type == Material_Finish::get("solid")) {
            result.depthRead = true;
            result.depthWrite = true;
        } else if (type == Material_Finish::get("translucent") ||
                   type == Material_Finish::get("shinny")) {
            result.depthRead = true;
            result.depthWrite = false;
        } else {
            assert(false);
            result.depthRead = true;
            result.depthWrite = true;
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