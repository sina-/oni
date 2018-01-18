#pragma once

#include <deque>
#include <GL/glew.h>
#include "graphics/renderer2d.h"
#include "graphics/renderable2d.h"


namespace granite {
	namespace graphics {
		class Simple2DRenderer : Renderer2D {
			std::deque<std::shared_ptr<Renderable2D>> m_RenderQueue;

		public:
			void submit(std::shared_ptr<Renderable2D> renderable) override;
			void flush() override;
		};
	}
}
