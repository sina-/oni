#pragma once

#include <oni-core/physics/physic.h>
#include <oni-core/physics/car.h>

namespace oni {
    namespace physics {
        class Movement : public Physic {
        public:
            void update(entities::World &world, const io::Input &input, float tickTime) override;
            void update(entities::Vehicle &vehicle, const io::Input &input, float tickTime) override;

        private:
            void updatePosition(components::Placement &position, const math::vec3 &direction, float tickTime);
        };
    }
}