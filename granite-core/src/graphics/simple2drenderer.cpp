#include "graphics/simple2drenderer.h"

namespace granite {
	namespace graphics {
		void Simple2DRenderer::submit(const Renderable2D * renderable)
		{
			m_RenderQueue.push_back(renderable);
		}

		void Simple2DRenderer::flush() 
		{
			while (!m_RenderQueue.empty()) {
				const Renderable2D * renderable = m_RenderQueue.front();

				auto vao = renderable->getVAO();
				auto ibo = renderable->getIBO();

				vao->bind();
				ibo->bind();

				renderable->getShader().setUniformMat4("ml_matrix", renderable->getModelMatrix());
				glDrawElements(GL_TRIANGLES, ibo->getCount(), GL_UNSIGNED_SHORT, nullptr);

				ibo->unbind();
				vao->unbind();

				m_RenderQueue.pop_front();
			}
		}

	}
}
