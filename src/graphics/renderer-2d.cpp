#include <oni-core/graphics/renderer-2d.h>

namespace oni {
    namespace graphics {
        Renderer2D::Renderer2D() {
            // identity matrix is the grand parent of all transformations, and
            // it should always stay in the stack.
            //mTransformationStack.push_back(math::mat4::identity());
        }

        Renderer2D::~Renderer2D() = default;

        void Renderer2D::begin() {
            _begin();
        }

        void Renderer2D::submit(const components::Shape &position, const components::Appearance &color) {
            _submit(position, color);
        }

        void Renderer2D::submit(const components::Shape &position, const components::Texture &texture) {
            _submit(position, texture);
        }

        void Renderer2D::submit(const components::Text &text) {
            _submit(text);
        }

        void Renderer2D::flush() {
            _flush();
        }

        void Renderer2D::end() {
            // TODO: probably you want to flush here
            _end();
        }
    }
}