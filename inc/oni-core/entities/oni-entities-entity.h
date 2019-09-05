#pragma once

#include<oni-core/common/oni-common-typedef.h>


namespace oni {
    // TODO: This should move to game
    enum class EntityType : oni::u16 {
        UNKNOWN,

        BACKGROUND,
        ROAD,
        WALL,

        RACE_CAR,
        VEHICLE,
        VEHICLE_GUN,
        VEHICLE_TIRE_REAR,
        VEHICLE_TIRE_FRONT,

        UI,
        CANVAS,

        SIMPLE_SPRITE,
        SIMPLE_PARTICLE,
        SIMPLE_BLAST_PARTICLE,
        SIMPLE_BLAST_ANIMATION,
        SIMPLE_ROCKET,

        TRAIL_PARTICLE,

        // TODO: This is not really an entity type, any entity can be rendered as a shinny! It really just effects
        // the blend function
                SHINNY_EFFECT,

        TEXT,
        WORLD_CHUNK,
        DEBUG_WORLD_CHUNK,

        SMOKE_CLOUD,

        COMPLEMENT,

        LAST
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

     bool
    operator==(const EntityPair &,
               const EntityPair &);

    struct EntityPairHasher {
        oni::size
        operator()(const EntityPair &) const noexcept;
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
}
