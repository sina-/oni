#pragma once

#include <oni-core/common/typedefs.h>
#include <oni-core/component/entity-definition.h>
#include <oni-core/math/vec3.h>

class b2Body;

namespace oni {
    namespace component {
        // TODO: Not sure if this is the right place to define this
        enum class WallTilePosition : common::uint8 {
            TOP = 1,
            RIGHT = 2,
            BOTTOM = 3,
            LEFT = 4,
        };

        enum class BodyType : common::uint8 {
            UNKNOWN = 0,
            STATIC = 1,
            KINEMATIC = 2,
            DYNAMIC = 3,
        };

        enum class PhysicalCategory : common::uint8 {
            UNKNOWN = 0,
            GENERIC = 1,
            VEHICLE = 2,
            RACE_CAR = 3,
            BULLET = 4,
            WALL = 5,
        };

        struct PhysicalProperties {
            common::real32 linearDamping{2.f};
            common::real32 angularDamping{2.f};
            common::real32 density{0.1f};
            common::real32 friction{1.0f};
            bool highPrecision{false};
            bool colliding{false};
            bool collisionWithinCategory{false}; // Determines if instances of this object can collide with each other
            PhysicalCategory physicalCategory{PhysicalCategory::UNKNOWN};
            BodyType bodyType{BodyType::UNKNOWN};
            // TODO: Not super happy about keeping a raw pointer to an object! But as long as I use naked Box2D
            // I don't think there is a better way.
            b2Body *body{nullptr};
        };

        struct CollidingEntity {
            EntityType entityA{EntityType::UNKNOWN};
            EntityType entityB{EntityType::UNKNOWN};
        };

        using CollisionPos = math::vec3;
    }
}
