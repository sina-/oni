#pragma once

#include <vector>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/entities/oni-entities-fwd.h>
#include <oni-core/entities/oni-entities-structure.h>
#include <oni-core/util/oni-util-hash.h>
#include <oni-core/util/oni-util-enum.h>


namespace oni {
    struct ComponentName : public HashedString {
    };

    struct EntityName : public Enum {
    };

    enum class SimMode : u8 {
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
        EntityName name{};
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
}

DEFINE_STD_HASH_FUNCTIONS(oni::ComponentName)

namespace std {
    template<>
    class hash<oni::EntityName> {
    public:
        constexpr size_t
        operator()(oni::EntityName const &hs) const {
            return hs.name.hash.value;
        }

        constexpr bool
        operator()(const oni::EntityName &lhs,
                   const oni::EntityName &rhs) const {
            return lhs.name.hash == rhs.name.hash;
        }
    };
}

