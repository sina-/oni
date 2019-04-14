#pragma once

#include <cmath>

#include <oni-core/common/typedefs.h>

/**
 * The idea is from https://github.com/spacejack/carphysics2d
 */

namespace oni {
    namespace component {
        class Car;

        class CarConfig;

        class CarInput;

    }
    namespace physics {

        void
        tickCar(component::Car &car,
                const component::CarConfig &config,
                const component::CarInput &inputs,
                common::real64 dt);


        common::CarSimDouble
        applySmoothSteer(const component::Car &car,
                         common::CarSimDouble steerInput,
                         common::real64 dt);

        common::CarSimDouble
        applySafeSteer(const component::Car &car,
                       common::CarSimDouble steerInput);

    }
}
