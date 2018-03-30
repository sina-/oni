#pragma once

#include <oni-core/entities/world.h>
#include <oni-core/graphics/window.h>
#include <oni-core/entities/vehicle.h>

namespace oni {
    namespace physics {
        class Physic {
        public:
            virtual void tick(entities::World &world, const io::Input &input, float tickTime) = 0;
            virtual void tick(entities::Vehicle &vehicle, const io::Input &input, float tickTime) = 0;
        };
    }
}