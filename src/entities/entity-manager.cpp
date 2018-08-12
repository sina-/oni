#include <oni-core/entities/entity-manager.h>


namespace oni {
    namespace entities {

        EntityManager::EntityManager() {
            mRegistry = std::make_unique<entt::DefaultRegistry>();
        }

        EntityManager::~EntityManager() = default;
    }
}