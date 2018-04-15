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

            Renderer2D() {
                // identity matrix is the grand parent of all transformations, and
                // it should always stay in the stack.
                //mTransformationStack.push_back(math::mat4::identity());
            }

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
            virtual void begin() = 0;

            virtual void submit(const components::Placement &position, const components::Appearance &color) = 0;

            virtual void submit(const components::Placement &position, const components::Texture &texture) = 0;
            virtual void
            submit(const components::Text &text) = 0;

            /**
             * Draw the element(s).
             */
            virtual void flush() = 0;

            /**
             * Clean up, if needed.
             */
            virtual void end() = 0;
        };
    }
}