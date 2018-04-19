#include <oni-core/entities/basic-entity-repo.h>

namespace oni {
    namespace entities {

        BasicEntityRepo::BasicEntityRepo() = default;

        common::entityID BasicEntityRepo::_createEntity() {
            if (!mFreeEntitySlots.empty()) {
                auto entity = mFreeEntitySlots.top();
                mFreeEntitySlots.pop();
                // Mark the memory location as free to over-write.
                mEntities[entity].set(components::READY);
                return entity;
            }

            mEntities.emplace_back(components::Mask().set(components::READY));
            // NOTE: This type of initialization will avoid copy construction.
            mPlacements.emplace_back(components::Placement{});
            mAppearances.emplace_back(components::Appearance{});
            mTextures.emplace_back(components::Texture{});
            mLayerIDs.emplace_back(0);
            mTexts.emplace_back(components::Text{});
            return mEntities.size() - 1;
        }

        void BasicEntityRepo::reserveEntity(size_t size) {
            mEntities.reserve(mEntities.size() + size);
            mPlacements.reserve(mPlacements.size() + size);
            mAppearances.reserve(mAppearances.size() + size);
            mTextures.reserve(mTextures.size() + size);
            mLayerIDs.reserve(mLayerIDs.size() + size);
            mTexts.reserve(mTexts.size() + size);
        }

        common::entityID
        BasicEntityRepo::createEntity(const components::Mask &mask, const components::LayerID &layerID) {
            auto entity = _createEntity();
            mEntities[entity] = mask;
            mLayerIDs[entity] = layerID;
            return entity;
        }

        void BasicEntityRepo::addComponent(common::entityID id, const components::Component &component) {
            mEntities[id].set(component);
        }

        void BasicEntityRepo::destroyEntity(common::entityID id) {
            mEntities[id] = components::Component::NONE;
            mFreeEntitySlots.push(id);
        }

        const components::EntityMasks &BasicEntityRepo::getEntities() const {
            return mEntities;
        }

        const components::Mask &BasicEntityRepo::getEntity(common::entityID id) const {
            return mEntities[id];
        }

        const components::Placement &BasicEntityRepo::getEntityPlacement(common::entityID id) const {
            return mPlacements[id];
        }

        components::Placement &BasicEntityRepo::getEntityPlacement(common::entityID id) {
            return mPlacements[id];
        }

        const components::Appearance &BasicEntityRepo::getEntityAppearance(common::entityID id) const {
            return mAppearances[id];
        }

        const components::Texture &BasicEntityRepo::getEntityTexture(common::entityID id) const {
            return mTextures[id];
        }

        const components::LayerID &BasicEntityRepo::getEntityLayerID(common::entityID id) const {
            return mLayerIDs[id];
        }

        const components::Text &BasicEntityRepo::getEntityText(common::entityID id) const {
            return mTexts[id];
        }

        void BasicEntityRepo::setEntityPlacement(common::entityID id, const components::Placement &placement) {
            mPlacements[id] = placement;
        }

        void BasicEntityRepo::setEntityAppearance(common::entityID id, const components::Appearance &appearance) {
            mAppearances[id] = appearance;
        }

        void BasicEntityRepo::setEntityTexture(common::entityID id, const components::Texture &texture) {
            mTextures[id] = texture;
        }

        void BasicEntityRepo::setEntityLayerID(common::entityID id, const components::LayerID &layerID) {
            mLayerIDs[id] = layerID;

        }

        void BasicEntityRepo::setEntityText(common::entityID id, const components::Text &text) {
            mTexts[id] = text;

        }

        components::Text &BasicEntityRepo::getEntityText(common::entityID id) {
            return mTexts[id];
        }
    }
}