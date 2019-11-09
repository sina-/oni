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

    // TODO: Did I settel down with ingeratance and not "using EntityName = HashedString"?
    // ImageName and SoundName have using ... = HashedString
    struct EntityName : public HashedString {
    };

    // TODO: Can I expose EnumBase and inherit from it so that users can define an enum for the entity name
    // if they want? :h
//    struct EntityName_ : public EnumBase {
//
//    };

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

        template<class Archive>
        void
        serialize(Archive &archive) {
            archive("id", id);
            archive("name", name);
        }
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
DEFINE_STD_HASH_FUNCTIONS(oni::EntityName)
