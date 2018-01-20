#pragma once

#include <deque>
#include <GL/glew.h>
#include "graphics/renderer2d.h"
#include "graphics/renderable2d.h"


namespace granite {
	namespace graphics {
		class Simple2DRenderer : Renderer2D {
			std::deque<std::shared_ptr<const Renderable2D>> m_RenderQueue;

		public:
			void submit(const std::shared_ptr<const Renderable2D> renderable) override;
			void flush() override;
		};
	}
}
