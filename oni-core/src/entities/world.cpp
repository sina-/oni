#include <entities/world.h>
#include <utils/io.h>

namespace oni {
    namespace entities {

        entities::World::World(unsigned long initialEntityCount) {
            m_Entities.reserve(initialEntityCount);
            m_Renderables.reserve(initialEntityCount);
        }

        auto World::createEntity() {
            if (!m_FreeEntitySlots.empty()) {
                auto entity = m_FreeEntitySlots.top();
                m_FreeEntitySlots.pop();
                // Mark the memory location as free to over-write.
                m_Entities[entity].set(components::RESERVED);
                return entity;
            }

            // Reserve big chunk to avoid reallocation.
            // NOTE: This should reserve every type of entity
            if (m_Entities.size() + 1 >= m_Entities.capacity()) {
                m_Entities.reserve(m_Entities.size() + EXPANSION_FACTOR);
                m_Renderables.reserve(m_Renderables.size() + EXPANSION_FACTOR);
            }

            m_Entities.emplace_back(components::Mask().set(components::RESERVED));
            m_Renderables.emplace_back(components::Renderable2D());

            return m_Entities.size() - 1;
        }

        void World::destroyEntity(unsigned long entity) {
            m_Entities[entity] = components::Component::NONE;
            m_FreeEntitySlots.push(entity);
        }

        unsigned long World::addCar(const components::Renderable2D &renderable2D) {
            auto entity = createEntity();
            m_Entities[entity] = components::Mask().set(components::SPRITE);
            m_Renderables[entity] = renderable2D;
            return entity;
        }

        const components::EntityMask &World::getEntities() const {
            return m_Entities;
        }

        const components::Mask &World::getEntity(unsigned long entity) const {
            return m_Entities[entity];
        }

        const components::Renderable2D &World::getRenderable2D(unsigned long entity) const {
            return m_Renderables[entity];
        }

    }
}