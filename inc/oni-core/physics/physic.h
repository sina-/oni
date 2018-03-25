#pragma once

#include <oni-core/entities/world.h>
#include <oni-core/graphics/window.h>

namespace oni {
    namespace physics {
        class Physic {
        public:
            // TODO: passing Window just to get the keyboard. Refactor Window for easier
            // access to IO events.
            virtual void update(entities::World &world, int keyPressed, float tickTime) = 0;
        };
    }
}