#pragma once

#include <deque>
#include <GL/glew.h>
#include "graphics/renderer2d.h"
#include "graphics/renderable2d.h"


namespace oni {
	namespace graphics {
		class Simple2DRenderer : Renderer2D {
//			std::deque<const std::unique_ptr<const Renderable2D>&> m_RenderQueue;

		public:
			void begin() {}
			void submit(const std::unique_ptr<Renderable2D> &renderable) override;
			void flush() override;
			void end() {}

		};
	}
}
