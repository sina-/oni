#pragma once

#include <oni-core/common/typedefs.h>
#include <oni-core/math/vec2.h>

class b2Body;

namespace oni {
    namespace component {
        // TODO: Not sure if this is the right place to define this
        enum class WallTilePosition : common::int8 {
            TOP = 1,
            RIGHT = 2,
            BOTTOM = 3,
            LEFT = 4,
        };

        enum class BodyType : common::int8 {
            STATIC = 1,
            KINEMATIC = 2,
            DYNAMIC = 3,
        };

        struct PhysicalProperties {
            common::real32 linearDamping{0.f};
            common::real32 angularDamping{0.f};
            common::real32 density{1.f};
            common::real32 friction{0.1f};
            bool bullet{false};
            bool colliding{false};
            BodyType bodyType{BodyType::STATIC};
            // TODO: Not super happy about keeping a raw pointer to an object! But as long as I use naked Box2D
            // I don't think there is a better way.
            b2Body *body{nullptr};
        };
    }
}
