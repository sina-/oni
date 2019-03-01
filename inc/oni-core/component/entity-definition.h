#pragma once

namespace oni {
    namespace component {
        enum class EntityType : common::uint8 {
            UNKNOWN = 0,

            BACKGROUND = 1,
            ROAD = 2,
            WALL = 3,

            RACE_CAR = 4,
            VEHICLE = 5,
            VEHICLE_GUN = 6,
            VEHICLE_TIRE = 7,

            SKID_LINE = 8,

            UI = 9,

            SIMPLE_SPRITE = 10,
            SIMPLE_PARTICLE = 11,
            SIMPLE_BULLET = 12,

            TEXT = 13,
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
