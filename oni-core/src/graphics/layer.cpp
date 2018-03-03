#include <graphics/layer.h>

namespace oni {
    namespace graphics {
        Layer::Layer(std::unique_ptr<Renderer2D> renderer, std::unique_ptr<Shader> shader, math::mat4 projectionMatrix)
                : m_Shader(std::move(shader)), m_Renderer2D(std::move(renderer)), m_ProjectionMatrix(projectionMatrix) {

            m_Shader->enable();
            m_Shader->setUniformMat4("pr_matrix", m_ProjectionMatrix);
            m_Shader->disable();
        }

        void Layer::renderSprites(const entities::World &world) {
            begin();

            unsigned long entityIndex = 0;
            for (const auto &entity: world.getEntities()) {
                // TODO: Use entities instead for faster calculations
                if (((entity & components::AppearanceComponent) == components::AppearanceComponent)
                    && (entity & components::PlacementComponent) == components::PlacementComponent
                    && world.getEntityShaderID(entityIndex).shaderID == m_Shader->getShaderID()) {

                    m_Renderer2D->submit(world.getEntityPlacement(entityIndex), world.getEntityAppearance(entityIndex));
                }
                ++entityIndex;
            }

            end();
        }

        void Layer::renderTexturedSprites(const entities::World &world) {
            begin();

            unsigned long entityIndex = 0;
            for (const auto &entity: world.getEntities()) {
                if (((entity & components::PlacementComponent) == components::PlacementComponent)
                    && ((entity & components::TextureComponent) == components::TextureComponent)
                    && world.getEntityShaderID(entityIndex).shaderID == m_Shader->getShaderID()) {

                    m_Renderer2D->submit(world.getEntityPlacement(entityIndex), world.getEntityAppearance(entityIndex),
                                         world.getEntityTexture(entityIndex));
                }
                ++entityIndex;
            }

            end();

        }

        const void Layer::begin() const {
            m_Shader->enable();
            m_Renderer2D->begin();
        }

        const void Layer::end() const {
            m_Renderer2D->end();
            m_Renderer2D->flush();
            m_Shader->disable();
        }
    }
}
