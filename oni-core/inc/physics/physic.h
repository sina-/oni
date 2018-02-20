#pragma once

#include <entities/world.h>
#include <graphics/window.h>

namespace oni {
    namespace physics {
        class Physic {
        public:
            // TODO: passing Window just to get the keyboard. Refactor Window for easier
            // access to IO events.
            virtual void update(entities::World &world, const graphics::Window &window) = 0;
        };
    }
}