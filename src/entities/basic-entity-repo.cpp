#include <oni-core/entities/basic-entity-repo.h>

namespace oni {
    namespace entities {

        BasicEntityRepo::BasicEntityRepo() = default;

        entities::entityID BasicEntityRepo::_createEntity() {
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
            mTextures.emplace_back("", 0, 0, 0, 0, 0, std::vector<math::vec2>());
            mLayerIDs.emplace_back(0);
            mTexts.emplace_back(components::Text());
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

        entities::entityID BasicEntityRepo::createEntity(const components::Mask &mask, const components::LayerID &layerID) {
            auto entity = _createEntity();
            mEntities[entity] = mask;
            mLayerIDs[entity] = layerID;
            return entity;
        }

        void BasicEntityRepo::addComponent(entities::entityID id, const components::Component &component) {
            mEntities[id].set(component);
        }

        void BasicEntityRepo::destroyEntity(entities::entityID id) {
            mEntities[id] = components::Component::NONE;
            mFreeEntitySlots.push(id);
        }

        const components::EntityMasks &BasicEntityRepo::getEntities() const {
            return mEntities;
        }

        const components::Mask &BasicEntityRepo::getEntity(entities::entityID id) const {
            return mEntities[id];
        }

        const components::Placement &BasicEntityRepo::getEntityPlacement(entities::entityID id) const {
            return mPlacements[id];
        }

        const components::Appearance &BasicEntityRepo::getEntityAppearance(entities::entityID id) const {
            return mAppearances[id];
        }

        const components::Texture &BasicEntityRepo::getEntityTexture(entities::entityID id) const {
            return mTextures[id];
        }

        const components::LayerID &BasicEntityRepo::getEntityLayerID(entities::entityID id) const {
            return mLayerIDs[id];
        }

        const components::Text &BasicEntityRepo::getEntityText(entities::entityID id) const {
            return mTexts[id];
        }

        void BasicEntityRepo::setEntityPlacement(entities::entityID id, const components::Placement &placement) {
            mPlacements[id] = placement;
        }

        void BasicEntityRepo::setEntityAppearance(entities::entityID id, const components::Appearance &appearance) {
            mAppearances[id] = appearance;
        }

        void BasicEntityRepo::setEntityTexture(entities::entityID id, const components::Texture &texture) {
            mTextures[id] = texture;
        }

        void BasicEntityRepo::setEntityLayerID(entities::entityID id, const components::LayerID &layerID) {
            mLayerIDs[id] = layerID;

        }

        void BasicEntityRepo::setEntityText(entities::entityID id, const components::Text &text) {
            mTexts[id] = text;

        }

        components::Text &BasicEntityRepo::getEntityText(entities::entityID id) {
            return mTexts[id];
        }
    }
}