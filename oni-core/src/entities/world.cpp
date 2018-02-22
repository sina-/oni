#include <entities/world.h>
#include <utils/io.h>

namespace oni {
    namespace entities {

        auto World::_createEntity() {
            if (!m_FreeEntitySlots.empty()) {
                auto entity = m_FreeEntitySlots.top();
                m_FreeEntitySlots.pop();
                // Mark the memory location as free to over-write.
                m_Entities[entity].set(components::READY);
                return entity;
            }

            m_Entities.emplace_back(components::Mask().set(components::READY));
            m_Placements.emplace_back(math::vec3(), math::vec3(), math::vec3(), math::vec3());
            m_Appearances.emplace_back(math::vec4());

            return m_Entities.size() - 1;
        }

        void World::reserveEntity(unsigned long size) {
            m_Entities.reserve(m_Entities.size() + size);
            m_Placements.reserve(m_Placements.size() + size);
            m_Appearances.reserve(m_Appearances.size() + size);
        }

        void World::destroyEntity(unsigned long entity) {
            m_Entities[entity] = components::Component::NONE;
            m_FreeEntitySlots.push(entity);
        }

        const components::EntityMask &World::getEntities() const {
            return m_Entities;
        }

        const components::Mask &World::getEntity(unsigned long entity) const {
            return m_Entities[entity];
        }

        const components::Placement &World::getEntityPlacement(unsigned long entity) const {
            return m_Placements[entity];
        }

        const components::Appearance &World::getEntityAppearance(unsigned long entity) const {
            return m_Appearances[entity];
        }

        unsigned long World::createEntity(components::Mask mask) {
            auto entity =_createEntity();
            m_Entities[entity] = mask;
            return entity;
        }

        void World::setEntityPlacement(unsigned long entity, components::Placement placement) {
            m_Placements[entity] = placement;
        }

        void World::setEntityAppearance(unsigned long entity, components::Appearance appearance) {
            m_Appearances[entity] = appearance;
        }


    }
}