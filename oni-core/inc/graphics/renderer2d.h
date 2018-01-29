#pragma once

#include "graphics/renderable2d.h"

namespace oni {
    namespace graphics {
        class Renderer2D {
        protected:
            /**
             * Preparation work, if needed.
             */
            virtual void begin() = 0;
            /**
			 * Renderable has to be shared_ptr if we are to use for multiple frames
			 * if it is unique_ptr then renderer needs to own it and retain it
			 * across frames.
             */
            virtual void submit(const std::unique_ptr<Renderable2D> &renderable) = 0;

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