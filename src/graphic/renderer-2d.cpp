#include <oni-core/graphic/renderer-2d.h>

namespace oni {
    namespace graphic {
        Renderer2D::Renderer2D() {
            // identity matrix is the grand parent of all transformations, and
            // it should always stay in the stack.
            //mTransformationStack.push_back(math::mat4::identity());
        }

        Renderer2D::~Renderer2D() = default;

        void Renderer2D::begin() {
            _begin();
        }

        void Renderer2D::submit(const component::Shape &position, const component::Appearance &color) {
            _submit(position, color);
        }

        void Renderer2D::submit(const component::Point &point, const component::Appearance &color, common::real32 time) {
            _submit(point, color, time);
        }

        void Renderer2D::submit(const component::Shape &position, const component::Texture &texture) {
            _submit(position, texture);
        }

        void Renderer2D::submit(const component::Text &text) {
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