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
            mPlacementsLocal.emplace_back(components::Placement{});
            mPlacementsWorld.emplace_back(components::Placement{});
            mAppearances.emplace_back(components::Appearance{});
            mTextures.emplace_back(components::Texture{});
            mShaderIDs.emplace_back(0);
            mTexts.emplace_back(components::Text{});
            return mEntities.size() - 1;
        }

        void BasicEntityRepo::reserveEntity(size_t size) {
            mEntities.reserve(mEntities.size() + size);
            mPlacementsLocal.reserve(mPlacementsLocal.size() + size);
            mPlacementsWorld.reserve(mPlacementsWorld.size() + size);
            mAppearances.reserve(mAppearances.size() + size);
            mTextures.reserve(mTextures.size() + size);
            mShaderIDs.reserve(mShaderIDs.size() + size);
            mTexts.reserve(mTexts.size() + size);
        }

        common::entityID
        BasicEntityRepo::createEntity(const components::Mask &mask, const components::ShaderID &shaderID) {
            auto entity = _createEntity();
            mEntities[entity] = mask;
            mShaderIDs[entity] = shaderID;
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

        const components::Placement &BasicEntityRepo::getEntityPlacementLocal(common::entityID id) const {
            return mPlacementsLocal[id];
        }

        components::Placement &BasicEntityRepo::getEntityPlacementLocal(common::entityID id) {
            return mPlacementsLocal[id];
        }

        const components::Appearance &BasicEntityRepo::getEntityAppearance(common::entityID id) const {
            return mAppearances[id];
        }

        const components::Texture &BasicEntityRepo::getEntityTexture(common::entityID id) const {
            return mTextures[id];
        }

        const components::ShaderID &BasicEntityRepo::getEntityShaderID(common::entityID id) const {
            return mShaderIDs[id];
        }

        const components::Text &BasicEntityRepo::getEntityText(common::entityID id) const {
            return mTexts[id];
        }

        void BasicEntityRepo::setEntityPlacementLocal(common::entityID id, const components::Placement &placement) {
            mPlacementsLocal[id] = placement;
        }

        void BasicEntityRepo::setEntityAppearance(common::entityID id, const components::Appearance &appearance) {
            mAppearances[id] = appearance;
        }

        void BasicEntityRepo::setEntityTexture(common::entityID id, const components::Texture &texture) {
            mTextures[id] = texture;
        }

        void BasicEntityRepo::setEntityShaderID(common::entityID id, const components::ShaderID &shaderID) {
            mShaderIDs[id] = shaderID;

        }

        void BasicEntityRepo::setEntityText(common::entityID id, const components::Text &text) {
            mTexts[id] = text;

        }

        components::Text &BasicEntityRepo::getEntityText(common::entityID id) {
            return mTexts[id];
        }

        void BasicEntityRepo::setEntityPlacementWorld(common::entityID id, const components::Placement &placement) {
            mPlacementsWorld[id] = placement;
        }

        const components::Placement &BasicEntityRepo::getEntityPlacementWorld(common::entityID id) const {
            return mPlacementsWorld[id];
        }

        components::Placement &BasicEntityRepo::getEntityPlacementWorld(common::entityID id) {
            return mPlacementsWorld[id];
        }
    }
}