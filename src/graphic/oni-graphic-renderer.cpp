#include <oni-core/graphic/oni-graphic-renderer.h>

#include <oni-core/component/oni-component-visual.h>


namespace oni {
    namespace graphic {
        Renderer::Renderer() = default;

        Renderer::~Renderer() = default;

        void
        Renderer::begin(const RenderSpec &spec) {
            assert(!mBegun);
            mBegun = true;

            if (spec.renderTarget) {
                mRenderTarget = spec.renderTarget;
                mViewportSize = getViewportSize();
                setViewportSize({spec.renderTarget->image.width, spec.renderTarget->image.height});
            }

            _begin(spec);
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

        Renderable::Renderable(common::EntityID _id,
                               const entities::EntityManager *_manager,
                               const component::WorldP3D *_pos,
                               const component::Heading *_heading,
                               const component::Scale *_scale,
                               const component::Color *_color,
                               const component::Texture *_texture,
                               const component::TextureAnimated *_animatedTexture) :
                id(_id),
                manager(_manager),
                pos(_pos),
                heading(_heading),
                scale(_scale),
                color(_color),
                texture(_texture),
                animatedTexture(
                        _animatedTexture) {}

    }
}