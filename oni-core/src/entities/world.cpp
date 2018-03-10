#include <entities/world.h>

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
            // NOTE: This type of initialization will avoid copy construction.
            m_Placements.emplace_back(math::vec3(), math::vec3(), math::vec3(), math::vec3());
            m_Appearances.emplace_back(math::vec4());
            m_Velocities.emplace_back(math::vec3(), 0.0f);
            m_Textures.emplace_back("", 0, 0, 0, std::vector<math::vec2>());
            m_LayerIDs.emplace_back(0);
            m_Texts.emplace_back(std::string(), math::vec3());
            return m_Entities.size() - 1;
        }

        void World::reserveEntity(unsigned long size) {
            m_Entities.reserve(m_Entities.size() + size);
            m_Placements.reserve(m_Placements.size() + size);
            m_Appearances.reserve(m_Appearances.size() + size);
            m_Velocities.reserve(m_Velocities.size() + size);
            m_Textures.reserve(m_Textures.size() + size);
            m_LayerIDs.reserve(m_LayerIDs.size() + size);
            m_Texts.reserve(m_Texts.size() + size);
        }

        unsigned long World::createEntity(const components::Mask &mask, const components::LayerID &layerID) {
            auto entity = _createEntity();
            m_Entities[entity] = mask;
            m_LayerIDs[entity] = layerID;
            return entity;
        }

        void World::addComponent(unsigned long entity, const components::Component &component) {
            m_Entities[entity].set(component);
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

        const components::Velocity &World::getEntityVelocity(unsigned long entity) const {
            return m_Velocities[entity];
        }

        const components::Texture &World::getEntityTexture(unsigned long entity) const {
            return m_Textures[entity];
        }

        const components::LayerID &World::getEntityLayerID(unsigned long entity) const {
            return m_LayerIDs[entity];
        }

        const components::Text &World::getEntityText(unsigned long entity) const {
            return m_Texts[entity];
        }

        void World::setEntityPlacement(unsigned long entity, const components::Placement &placement) {
            m_Placements[entity] = placement;
        }

        void World::setEntityAppearance(unsigned long entity, const components::Appearance &appearance) {
            m_Appearances[entity] = appearance;
        }

        void World::setEntityVelocity(unsigned long entity, const components::Velocity &velocity) {
            m_Velocities[entity] = velocity;
        }

        void World::setEntityTexture(unsigned long entity, const components::Texture &texture) {
            m_Textures[entity] = texture;
        }

        void World::setEntityLayerID(unsigned long entity, const components::LayerID &layerID) {
            m_LayerIDs[entity] = layerID;

        }

        void World::setEntityText(unsigned long entity, const components::Text &text) {
            m_Texts[entity] = text;

        }
    }
}