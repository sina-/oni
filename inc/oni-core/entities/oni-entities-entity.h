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

            TEXT,
            WORLD_CHUNK,
            DEBUG_WORLD_CHUNK,

            SMOKE,

            COMPLEMENT,

            LAST
        };

        enum class SimMode : common::u8 {
            CLIENT,
            SERVER,
        };

        enum class SnapshotType {
            ONLY_COMPONENTS = 1,
            ONLY_NEW_ENTITIES = 2,
            ENTIRE_REGISTRY = 3,
        };

        struct DeletedEntity {
            entities::EntityType type;
            common::EntityID entity;
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
        };

    }
}
