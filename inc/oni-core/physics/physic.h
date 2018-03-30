#pragma once

#include <oni-core/entities/basic-entity-repo.h>
#include <oni-core/graphics/window.h>
#include <oni-core/entities/vehicle-entity-repo.h>

namespace oni {
    namespace physics {
        class Physic {
        public:
            virtual void tick(entities::VehicleEntityRepo &vehicle, const io::Input &input, float tickTime) = 0;
        };
    }
}