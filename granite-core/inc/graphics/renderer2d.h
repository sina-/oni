#pragma once

#include "graphics/renderable2d.h"

namespace granite {
	namespace graphics {
		/*
		 * Buffer list of Renderable2D objects. Draw them all together with flush().
		 */
		class Renderer2D {
		protected:
			virtual void submit(const Renderable2D* renderable) = 0;
			virtual void flush() = 0;
		};
	}
}