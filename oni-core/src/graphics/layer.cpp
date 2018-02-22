#include <graphics/layer.h>

namespace oni {
    namespace graphics {
        Layer::Layer(std::unique_ptr<Renderer2D> renderer, std::unique_ptr<Shader> shader, math::mat4 projectionMatrix)
                : m_Shader(std::move(shader)), m_Renderer2D(std::move(renderer)), m_ProjectionMatrix(projectionMatrix) {

            m_Shader->enable();
            m_Shader->setUniformMat4("pr_matrix", m_ProjectionMatrix);
            m_Shader->disable();
        }

        void Layer::render(const entities::World &world, const components::Mask &mask) {
            m_Shader->enable();
            m_Renderer2D->begin();

            unsigned long entityIndex = 0;
            for (const auto &entity: world.getEntities()) {
                if ((entity & mask) == mask) {
                    m_Renderer2D->submit(world.getEntityPlacement(entityIndex), world.getEntityAppearance(entityIndex));
                }
                ++entityIndex;
            }
            m_Renderer2D->end();
            m_Renderer2D->flush();
            m_Shader->disable();

        }
    }
}
