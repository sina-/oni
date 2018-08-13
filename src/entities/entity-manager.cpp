#include <oni-core/entities/entity-manager.h>


namespace oni {
    namespace entities {

        EntityManager::EntityManager() {
            mRegistry = std::make_unique<entt::DefaultRegistry>();
        }

        common::EntityID EntityManager::create() {
            return mRegistry->create();
        }

        size_t EntityManager::size() const noexcept {
            return mRegistry->size();
        }

        EntityManager::~EntityManager() = default;
    }
}