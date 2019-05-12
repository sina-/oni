#pragma once

namespace oni {
    namespace entities {
        enum class EntityType : common::uint16 {
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


            LAST
        };

        enum class EntityClass : common::uint8 {
            CLIENT_SIDE,
            SERVER_SIDE,
            CLIENT_AND_SERVER_SIDE
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
        };
    }
}
