#pragma once

#include <oni-core/physics/physic.h>

namespace oni {
    namespace physics {
        class Dynamics : public Physic {

        public:
            ~Dynamics() override = default;

            void tick(entt::DefaultRegistry &registry, const io::Input &input, common::real32 tickTime) override;
        };
    }
}