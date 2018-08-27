#pragma once

#include <cmath>
#include <algorithm>

#include <oni-core/common/typedefs.h>

/**
 * The idea is from https://github.com/spacejack/carphysics2d
 */

namespace oni {
    namespace components {
        class Car;

        class CarConfig;

        class CarInput;

    }
    namespace physics {

        template<class T>
        common::int32 sign(T n) {
            return (T(0) < n) - (T(0) > n);
        }

        template<class T>
        T clip(const T &n, const T &lower, const T &upper) {
            return std::max(lower, std::min(n, upper));
        }

        void tickCar(components::Car &car, const components::CarConfig &config,
                     const components::CarInput &inputs, common::real64 dt);


        common::CarSimDouble applySmoothSteer(const components::Car &car,
                                              common::CarSimDouble steerInput, common::real64 dt);

        common::CarSimDouble applySafeSteer(const components::Car &car,
                                                common::CarSimDouble steerInput);

    }
}
