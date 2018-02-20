#pragma once

#include <components/render-components.h>

namespace oni {
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
            std::vector<math::mat4> m_TransformationStack;

            Renderer2D() {
                // identity matrix is the grand parent of all transformations, and
                // it should always stay in the stack.
                m_TransformationStack.push_back(math::mat4::identity());
            }

            void push(const math::mat4 transformation) {
                m_TransformationStack.push_back(m_TransformationStack.back() * transformation);
            }

            void pop() {
                if (m_TransformationStack.size() > 1) {
                    m_TransformationStack.pop_back();
                }
            }

        public:
            /**
             * Preparation work, if needed.
             */
            virtual void begin() = 0;

            virtual void submit(const components::Position &position, const components::Appearance &color) = 0;

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