#pragma once

#include <oni-core/physics/physic.h>

namespace oni {
    namespace entities {
        class VehicleEntityRepo;
    }

    namespace physics {
        class Dynamics : public Physic {

        public:
            ~Dynamics() = default;

            void tick(entities::VehicleEntityRepo &vehicle, const io::Input &input, float tickTime) override;
        };
    }
}