#pragma once

namespace oni {

    namespace component {
        class Appearance;

        class Texture;

        class Text;

        class Shape;
    }

    namespace graphic {
        class Renderer2D {
        protected:
            Renderer2D();

            virtual ~Renderer2D();

        public:
            /**
             * Preparation work, if needed.
             */
            void begin();

            void submit(const component::Shape &position, const component::Appearance &color);

            void submit(const component::Shape &position, const component::Texture &texture);

            void submit(const component::Text &text);

            /**
             * Draw the element(s).
             */
            void flush();

            /**
             * Clean up, if needed.
             */
            void end();

        private:
            virtual void _begin() = 0;

            virtual void _submit(const component::Shape &position, const component::Appearance &color) = 0;

            virtual void _submit(const component::Shape &position, const component::Texture &texture) = 0;

            virtual void _submit(const component::Text &text) = 0;

            /**
             * Draw the element(s).
             */
            virtual void _flush() = 0;

            /**
             * Clean up, if needed.
             */
            virtual void _end() = 0;
        };
    }
}