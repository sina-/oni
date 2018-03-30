#pragma once

#include <oni-core/entities/basic-entity-repo.h>
#include <oni-core/graphics/window.h>
#include <oni-core/entities/vehicle.h>

namespace oni {
    namespace physics {
        class Physic {
        public:
            virtual void tick(entities::BasicEntityRepo &basicEntityRepo, const io::Input &input, float tickTime) = 0;
            virtual void tick(entities::Vehicle &vehicle, const io::Input &input, float tickTime) = 0;
        };
    }
}