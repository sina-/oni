#include <oni-core/graphic/oni-graphic-renderer-2d.h>

namespace oni {
    namespace graphic {
        Renderer2D::Renderer2D() = default;

        Renderer2D::~Renderer2D() = default;

        void
        Renderer2D::begin() {
            _begin();
        }

        void
        Renderer2D::submit(const component::WorldP3D &pos,
                           const component::Heading &heading,
                           const component::Scale &scale,
                           const component::Appearance &apperance,
                           const component::Texture &texture) {
            _submit(pos, heading, scale, apperance, texture);
        }

        void
        Renderer2D::submit(const component::Text &text,
                           const component::WorldP3D &pos) {
            _submit(text, pos);
        }

        void
        Renderer2D::flush() {
            _flush();
        }

        void
        Renderer2D::end() {
            // TODO: probably you want to flush here
            _end();
        }
    }
}