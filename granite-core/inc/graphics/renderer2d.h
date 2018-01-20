#pragma once

#include "graphics/renderable2d.h"

namespace granite {
	namespace graphics {
		/*
		 * Buffer list of Renderable2D objects. Draw them all together with flush().
		 */
		class Renderer2D {
		protected:
            // renderable has to be shared_ptr if we are to use for multiple frames
            // if it is unique_ptr then renderer needs to own it and retain it
			// across frames.
			virtual void submit(const std::shared_ptr<const Renderable2D> renderable) = 0;
			virtual void flush() = 0;
		};
	}
}