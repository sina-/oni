#include <oni-core/entities/world.h>

namespace oni {
    namespace entities {

        auto World::_createEntity() {
            if (!mFreeEntitySlots.empty()) {
                auto entity = mFreeEntitySlots.top();
                mFreeEntitySlots.pop();
                // Mark the memory location as free to over-write.
                mEntities[entity].set(components::READY);
                return entity;
            }

            mEntities.emplace_back(components::Mask().set(components::READY));
            // NOTE: This type of initialization will avoid copy construction.
            mPlacements.emplace_back(math::vec3(), math::vec3(), math::vec3(), math::vec3());
            mAppearances.emplace_back(math::vec4());
            mVelocities.emplace_back(math::vec3(), 0.0f);
            mTextures.emplace_back("", 0, 0, 0, std::vector<math::vec2>());
            mLayerIDs.emplace_back(0);
            mTexts.emplace_back(components::Text());
            return mEntities.size() - 1;
        }

        void World::reserveEntity(entities::entityID size) {
            mEntities.reserve(mEntities.size() + size);
            mPlacements.reserve(mPlacements.size() + size);
            mAppearances.reserve(mAppearances.size() + size);
            mVelocities.reserve(mVelocities.size() + size);
            mTextures.reserve(mTextures.size() + size);
            mLayerIDs.reserve(mLayerIDs.size() + size);
            mTexts.reserve(mTexts.size() + size);
        }

        entities::entityID World::createEntity(const components::Mask &mask, const components::LayerID &layerID) {
            auto entity = _createEntity();
            mEntities[entity] = mask;
            mLayerIDs[entity] = layerID;
            return entity;
        }

        void World::addComponent(entities::entityID id, const components::Component &component) {
            mEntities[id].set(component);
        }

        void World::destroyEntity(entities::entityID id) {
            mEntities[id] = components::Component::NONE;
            mFreeEntitySlots.push(id);
        }

        const components::EntityMask &World::getEntities() const {
            return mEntities;
        }

        const components::Mask &World::getEntity(entities::entityID id) const {
            return mEntities[id];
        }

        const components::Placement &World::getEntityPlacement(entities::entityID id) const {
            return mPlacements[id];
        }

        const components::Appearance &World::getEntityAppearance(entities::entityID id) const {
            return mAppearances[id];
        }

        const components::Velocity &World::getEntityVelocity(entities::entityID id) const {
            return mVelocities[id];
        }

        const components::Texture &World::getEntityTexture(entities::entityID id) const {
            return mTextures[id];
        }

        const components::LayerID &World::getEntityLayerID(entities::entityID id) const {
            return mLayerIDs[id];
        }

        const components::Text &World::getEntityText(entities::entityID id) const {
            return mTexts[id];
        }

        void World::setEntityPlacement(entities::entityID id, const components::Placement &placement) {
            mPlacements[id] = placement;
        }

        void World::setEntityAppearance(entities::entityID id, const components::Appearance &appearance) {
            mAppearances[id] = appearance;
        }

        void World::setEntityVelocity(entities::entityID id, const components::Velocity &velocity) {
            mVelocities[id] = velocity;
        }

        void World::setEntityTexture(entities::entityID id, const components::Texture &texture) {
            mTextures[id] = texture;
        }

        void World::setEntityLayerID(entities::entityID id, const components::LayerID &layerID) {
            mLayerIDs[id] = layerID;

        }

        void World::setEntityText(entities::entityID id, const components::Text &text) {
            mTexts[id] = text;

        }
    }
}