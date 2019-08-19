#pragma once

#include <cmath>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-fwd.h>
#include <oni-core/io/oni-io-fwd.h>


/**
 * The idea is from https://github.com/sina-/carphysics2d
 */

namespace oni {
    void
    tickCar(Car &car,
            WorldP3D &,
            Heading &,
            const CarConfig &config,
            const CarInput &inputs,
            r64 dt);


    r64
    applySmoothSteer(const Car &car,
                     r64 steerInput,
                     r64 dt);

    r64
    applySafeSteer(const Car &car,
                   r64 steerInput);

}
