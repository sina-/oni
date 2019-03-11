#pragma once

namespace oni {
    namespace component {
        enum class EntityType : common::uint8 {
            UNKNOWN,

            BACKGROUND,
            ROAD,
            WALL,

            RACE_CAR,
            VEHICLE,
            VEHICLE_GUN,
            VEHICLE_TIRE_REAR,
            VEHICLE_TIRE_FRONT,

            SKID_LINE,

            UI,

            SIMPLE_SPRITE,
            SIMPLE_PARTICLE,
            SIMPLE_BULLET,

            TEXT,
            WORLD_CHUNK,
        };

        enum class SnapshotType {
            ONLY_COMPONENTS = 1,
            ONLY_NEW_ENTITIES = 2,
            ENTIRE_REGISTRY = 3,
        };

        struct DeletedEntity {
            component::EntityType type;
            common::EntityID entity;
        };
    }
}
