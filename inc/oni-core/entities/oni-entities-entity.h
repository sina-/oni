#pragma once

#include<oni-core/common/oni-common-typedef.h>

namespace oni {
    namespace entities {
        enum class EntityType : common::u16 {
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
            SIMPLE_ROCKET,

            TRAIL_PARTICLE,

            TEXT,
            WORLD_CHUNK,
            DEBUG_WORLD_CHUNK,

            SMOKE_CLOUD,

            COMPLEMENT,

            LAST
        };

        enum class SimMode : common::u8 {
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
            common::EntityID id;
            entities::EntityType type;
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
}
