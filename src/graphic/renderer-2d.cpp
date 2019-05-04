#include <oni-core/graphic/renderer-2d.h>

namespace oni {
    namespace graphic {
        Renderer2D::Renderer2D() = default;

        Renderer2D::~Renderer2D() = default;

        void
        Renderer2D::begin() {
            _begin();
        }

        void
        Renderer2D::submit(const component::Shape &position,
                           const component::Appearance &color) {
            _submit(position, color);
        }

        void
        Renderer2D::submit(const component::Tessellation &tessellation,
                           const component::WorldP3D &pos,
                           const component::Heading &heading,
                           const component::Age &age,
                           const component::Velocity &velocity,
                           const component::Appearance &color) {
            _submit(tessellation, pos, heading, age, velocity, color);
        }

        void
        Renderer2D::submit(const component::Tessellation &tessellation,
                           const component::WorldP3D &pos,
                           const component::Heading &heading,
                           const component::Age &age,
                           const component::Velocity &velocity,
                           const component::Texture &texture) {
            _submit(tessellation, pos, heading, age, velocity, texture);
        }

        void
        Renderer2D::submit(const component::Shape &position,
                           const component::Texture &texture) {
            _submit(position, texture);
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