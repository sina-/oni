#pragma once

namespace oni {

    namespace components {
        class Appearance;

        class Texture;

        class Text;

        class Shape;
    }

    namespace graphics {
        class Renderer2D {
        protected:
            Renderer2D();

            virtual ~Renderer2D();

        public:
            /**
             * Preparation work, if needed.
             */
            void begin();

            void submit(const components::Shape &position, const components::Appearance &color);

            void submit(const components::Shape &position, const components::Texture &texture);

            void submit(const components::Text &text);

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

            virtual void _submit(const components::Shape &position, const components::Appearance &color) = 0;

            virtual void _submit(const components::Shape &position, const components::Texture &texture) = 0;

            virtual void _submit(const components::Text &text) = 0;

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