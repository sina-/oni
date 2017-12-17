#pragma once

#include <deque>
#include <glew.h>
#include "renderer2d.h"
#include "renderable2d.h"


namespace granite {
	namespace graphics {
		class Simple2DRenderer : Renderer2D {
			std::deque<const Renderable2D*> m_RenderQueue;

		public:
			void submit(const Renderable2D* renderable) override;
			void flush() override;
		};
	}
}
