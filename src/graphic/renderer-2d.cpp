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
        Renderer2D::submit(const component::Particle &particle,
                           const component::Placement &placement,
                           const component::Age &age,
                           const component::Velocity &velocity,
                           const component::Appearance &color) {
            _submit(particle, placement, age, velocity, color);
        }

        void
        Renderer2D::submit(const component::Particle &particle,
                           const component::Placement &placement,
                           const component::Age &age,
                           const component::Velocity &velocity,
                           const component::Texture &texture) {
            _submit(particle, placement, age, velocity, texture);
        }

        void
        Renderer2D::submit(const component::Shape &position,
                           const component::Texture &texture) {
            _submit(position, texture);
        }

        void
        Renderer2D::submit(const component::Text &text) {
            _submit(text);
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