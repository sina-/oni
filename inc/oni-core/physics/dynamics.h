#pragma once

#include <oni-core/physics/physic.h>
#include <oni-core/physics/car.h>

namespace oni {
    namespace physics {
        class Dynamics : public Physic {
        public:
            void tick(entities::VehicleEntityRepo &vehicle, const io::Input &input, float tickTime) override;
        };
    }
}