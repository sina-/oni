#pragma once

namespace oni {
    namespace entities {
        class VehicleEntityRepo;
    }

    namespace io {
        class Input;
    }

    namespace physics {
        class Physic {
        protected:
            virtual ~Physic() = default;

        public:
            virtual void tick(entities::VehicleEntityRepo &vehicle, const io::Input &input, float tickTime) = 0;
        };
    }
}