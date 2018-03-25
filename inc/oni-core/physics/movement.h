#pragma once

#include <oni-core/physics/physic.h>

namespace oni {
    namespace physics {
        class Movement : public Physic {
        public:
            void update(entities::World &world, int keyPressed, float tickTime) override;

        private:
            void updatePosition(components::Placement &position, const math::vec3 &direction, float tickTime);
        };
    }
}