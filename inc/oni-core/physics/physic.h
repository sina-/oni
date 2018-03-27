#pragma once

#include <oni-core/entities/world.h>
#include <oni-core/graphics/window.h>
#include <oni-core/entities/vehicle.h>

namespace oni {
    namespace physics {
        class Physic {
        public:
            virtual void update(entities::World &world, int keyPressed, float tickTime) = 0;
            virtual void update(entities::Vehicle &vehicle, int keyPressed, float tickTime) = 0;
        };
    }
}