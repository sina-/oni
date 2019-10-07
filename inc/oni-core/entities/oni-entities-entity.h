#pragma once

#include <vector>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/entities/oni-entities-fwd.h>


namespace oni {
    struct EntityType {
        oni::u16 value;
    };

    enum class SimMode : oni::u8 {
        UNKNOWN,

        CLIENT,
        SERVER,
        CLIENT_SIDE_SERVER,

        LAST
    };

    enum class SnapshotType {
        ONLY_COMPONENTS = 1,
        ONLY_NEW_ENTITIES = 2,
        ENTIRE_REGISTRY = 3,
    };

    struct DeletedEntity {
        EntityID id{};
        EntityType type{};
    };

    struct EntityPair {
        EntityID a{};
        EntityID b{};
    };

    struct EntityOperationPolicy {
        bool track{true};
        bool safe{false};

        inline static EntityOperationPolicy
        client() {
            auto policy = EntityOperationPolicy{};
            policy.track = false;
            policy.safe = false;
            return policy;
        }

        inline static EntityOperationPolicy
        server() {
            auto policy = EntityOperationPolicy{};
            policy.track = true;
            return policy;
        }

        inline static EntityOperationPolicy
        clientServer() {
            auto policy = EntityOperationPolicy{};
            policy.track = false;
            policy.safe = true;
            return policy;
        }
    };

    struct EntityContext {
        EntityManager *mng;
        EntityID id;
    };

    struct EntityTickContext {
        EntityManager &mng;
        EntityID id{};
        duration64 dt{};
    };

    struct BindLifetimeParent {
        std::vector<EntityContext> children;
    };

    struct BindLifetimeChild {
        // NOTE: Mostly useful for debugging
        EntityContext parent;
    };

    template<class T>
    struct AttachedComponent {
        T component;
        EntityContext attachee;
    };
}
