#pragma once

#include <cmath>
#include <algorithm>

#include <oni-core/math/vec2.h>
#include <oni-core/components/input-data.h>
#include <oni-core/components/physical.h>

/**
 * The idea is from https://github.com/spacejack/carphysics2d
 */

namespace oni {
    namespace physics {

        template<class T>
        int sign(T n) {
            return (T(0) < n) - (T(0) > n);
        }

        template<class T>
        T clip(const T &n, const T &lower, const T &upper) {
            return std::max(lower, std::min(n, upper));
        }

        void tickCar(components::Car &car, const components::CarConfig &config,
                     const components::CarInput &inputs, float dt);


        components::carSimDouble applySmoothSteer(const components::Car &car,
                                                  components::carSimDouble steerInput, float dt);

        components::carSimDouble applySafeSteer(const components::Car &car,
                                                components::carSimDouble steerInput);

    }
}
