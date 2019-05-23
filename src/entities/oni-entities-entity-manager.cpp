#include <oni-core/entities/oni-entities-manager.h>

namespace oni {
    namespace entities {

        EntityManager::EntityManager() {
            mRegistry = std::make_unique<entt::basic_registry<common::u32 >>();
            mLoader = std::make_unique<entt::basic_continuous_loader<common::EntityID>>(*mRegistry);
            mDispatcher = std::make_unique<entt::dispatcher>();
        }

        EntityManager::~EntityManager() = default;

        void
        EntityManager::clearDeletedEntitiesList() {
            mDeletedEntities.clear();
        }

        common::EntityID
        EntityManager::createComplementTo(common::EntityID id) {
            assert(mComplementaryEntities.find(id) == mComplementaryEntities.end());
            auto result = create();
            assign<entities::EntityType>(result, entities::EntityType::COMPLEMENT);
            mComplementaryEntities[id] = result;
            return result;
        }

        common::EntityID
        EntityManager::getComplementOf(common::EntityID id) {
            assert(mComplementaryEntities.find(id) != mComplementaryEntities.end());
            return mComplementaryEntities[id];
        }

        bool
        EntityManager::hasComplement(common::EntityID id) {
            return mComplementaryEntities.find(id) != mComplementaryEntities.end();
        }

        size_t
        EntityManager::size() noexcept {
            auto result = mRegistry->size();
            return result;
        }

        common::EntityID
        EntityManager::map(common::EntityID entityID) {
            auto result = mLoader->map(entityID);
            if (result == entt::null) {
                return 0;
            }
            return result;
        }

        const std::vector<common::EntityID> &
        EntityManager::getDeletedEntities() const {
            return mDeletedEntities;
        }

        void
        EntityManager::tagForComponentSync(common::EntityID entity) {
            accommodate<component::Tag_OnlyComponentUpdate>(entity);
        }

        void
        EntityManager::dispatchEvents() {
            mDispatcher->update();
        }

        common::EntityID
        EntityManager::create() {
            auto result = mRegistry->create();
            return result;
        }

        void
        EntityManager::destroy(common::EntityID entityID) {
            mRegistry->destroy(entityID);
        }

        void
        EntityManager::destroyAndTrack(common::EntityID entityID) {
            mRegistry->destroy(entityID);
            mDeletedEntities.push_back(entityID);
        }

        bool
        EntityManager::valid(common::EntityID entityID) {
            return mRegistry->valid(entityID);
        }
    }
}