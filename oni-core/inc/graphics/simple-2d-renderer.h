#pragma once

#include <deque>
#include <GL/glew.h>
#include "graphics/renderer-2d.h"


namespace oni {
	namespace graphics {
		class Simple2DRenderer : Renderer2D {
//			std::deque<const std::unique_ptr<const Sprite>&> m_RenderQueue;

		public:
			void begin() {}
			void submit(const components::Position &position, const components::Appearance &appearance) override;
			void flush() override;
			void end() {}

		};
	}
}
