#include "graphics/simple2drenderer.h"

namespace oni {
	namespace graphics {
		void Simple2DRenderer::submit(std::shared_ptr<const Renderable2D> renderable)
		{
			m_RenderQueue.push_back(renderable);
		}

		void Simple2DRenderer::flush() 
		{
            // TODO: One draw call per object. Quite bad. Better batch draw bunch of them.
			while (!m_RenderQueue.empty()) {
				auto renderable = m_RenderQueue.front().get();

/*				auto vao = renderable->getVAO();
				auto ibo = renderable->getIBO();

				vao->bind();
				ibo->bind();*/

//				renderable->getShader().setUniformMat4("ml_matrix", renderable->getModelMatrix());
                // TODO: Store the vertex type, GL_UNSIGNED_SHORT, as the part of renderable.
//				glDrawElements(GL_TRIANGLES, ibo->getCount(), GL_UNSIGNED_SHORT, nullptr);

/*				ibo->unbind();
				vao->unbind();*/

				// Odd, but queue has no method to remove and return the front element, therefore
                // have to remove it manually after use.
				m_RenderQueue.pop_front();
			}
		}

	}
}
