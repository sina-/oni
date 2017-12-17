#pragma once

#include "renderable2d.h"

namespace granite {
	namespace graphics {
		class Renderer2D {
		protected:
			virtual void submit(const Renderable2D* renderable) = 0;
			virtual void flush() = 0;
		};
	}
}