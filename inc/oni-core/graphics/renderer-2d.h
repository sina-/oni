#pragma once

namespace oni {

    namespace components {
        class Appearance;

        class Texture;

        class Text;

        class Placement;
    }
    namespace graphics {
        class Renderer2D {
        protected:
            /**
             * If the objects to be rendered has to be transformed together
             * we have to keep list of such transformations to be applied in
             * the reverse order. For example, if a car and its headlight has to
             * move to point A, and headlights position is defined relative to the car,
             * first the car moves to A, then headlight moves to A
             * and continues to move farther to its location relative to the car.
             * In other word, a child has to apply all the transformations of its
             * parents prior to its own transformation.
             */
            //std::vector<math::mat4> mTransformationStack;

            Renderer2D();

            virtual ~Renderer2D();

/*            void push(const math::mat4 transformation) {
                mTransformationStack.push_back(mTransformationStack.back() * transformation);
            }

            void pop() {
                if (mTransformationStack.size() > 1) {
                    mTransformationStack.pop_back();
                }
            }*/

        public:
            /**
             * Preparation work, if needed.
             */
            void begin();

            void submit(const components::Placement &position, const components::Appearance &color);

            void submit(const components::Placement &position, const components::Texture &texture);

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

            virtual void _submit(const components::Placement &position, const components::Appearance &color) = 0;

            virtual void _submit(const components::Placement &position, const components::Texture &texture) = 0;

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