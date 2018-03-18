#include <entities/world.h>

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

        void World::reserveEntity(unsigned long size) {
            mEntities.reserve(mEntities.size() + size);
            mPlacements.reserve(mPlacements.size() + size);
            mAppearances.reserve(mAppearances.size() + size);
            mVelocities.reserve(mVelocities.size() + size);
            mTextures.reserve(mTextures.size() + size);
            mLayerIDs.reserve(mLayerIDs.size() + size);
            mTexts.reserve(mTexts.size() + size);
        }

        unsigned long World::createEntity(const components::Mask &mask, const components::LayerID &layerID) {
            auto entity = _createEntity();
            mEntities[entity] = mask;
            mLayerIDs[entity] = layerID;
            return entity;
        }

        void World::addComponent(unsigned long entity, const components::Component &component) {
            mEntities[entity].set(component);
        }

        void World::destroyEntity(unsigned long entity) {
            mEntities[entity] = components::Component::NONE;
            mFreeEntitySlots.push(entity);
        }

        const components::EntityMask &World::getEntities() const {
            return mEntities;
        }

        const components::Mask &World::getEntity(unsigned long entity) const {
            return mEntities[entity];
        }

        const components::Placement &World::getEntityPlacement(unsigned long entity) const {
            return mPlacements[entity];
        }

        const components::Appearance &World::getEntityAppearance(unsigned long entity) const {
            return mAppearances[entity];
        }

        const components::Velocity &World::getEntityVelocity(unsigned long entity) const {
            return mVelocities[entity];
        }

        const components::Texture &World::getEntityTexture(unsigned long entity) const {
            return mTextures[entity];
        }

        const components::LayerID &World::getEntityLayerID(unsigned long entity) const {
            return mLayerIDs[entity];
        }

        const components::Text &World::getEntityText(unsigned long entity) const {
            return mTexts[entity];
        }

        void World::setEntityPlacement(unsigned long entity, const components::Placement &placement) {
            mPlacements[entity] = placement;
        }

        void World::setEntityAppearance(unsigned long entity, const components::Appearance &appearance) {
            mAppearances[entity] = appearance;
        }

        void World::setEntityVelocity(unsigned long entity, const components::Velocity &velocity) {
            mVelocities[entity] = velocity;
        }

        void World::setEntityTexture(unsigned long entity, const components::Texture &texture) {
            mTextures[entity] = texture;
        }

        void World::setEntityLayerID(unsigned long entity, const components::LayerID &layerID) {
            mLayerIDs[entity] = layerID;

        }

        void World::setEntityText(unsigned long entity, const components::Text &text) {
            mTexts[entity] = text;

        }
    }
}