#include <oni-core/physics/oni-physics-system.h>

#include <oni-core/io/oni-io-input.h>
#include <oni-core/physics/oni-physics-car.h>

namespace oni {
    System_Car::System_Car(EntityManager &em) : SystemTemplate(em) {}

    void
    System_Car::update(EntityTickContext &etc,
                       Car &car,
                       CarInput &input,
                       CarConfig &cc,
                       WorldP3D &pos,
                       Orientation &ornt) {
        auto steerInput = input.left - input.right;
        if (car.smoothSteer) {
            car.steer = applySmoothSteer(car, steerInput, etc.dt);
        } else {
            car.steer = steerInput;
        }

        if (car.safeSteer) {
            car.steer = applySafeSteer(car, steerInput);
        }

        car.steerAngle = car.steer * cc.maxSteer;
        if (input.nitro) {
            car.velocity += vec2{static_cast<r32>(cos(ornt.value)),
                                 static_cast<r32>(sin(ornt.value))};
        }

        const auto oldPos = pos;
        const auto oldOrnt = ornt;
        tickCar(car, pos, ornt, cc, input, etc.dt);

        // TODO: This is probably not needed, client should be able to figure this out.
        auto velocity = car.velocityLocal.len();
        auto distanceFromCamera = 1 + velocity * 2 / car.maxVelocityAbsolute;
        if (!almost_Equal(oldPos.x, pos.x) ||
            !almost_Equal(oldPos.y, pos.y) ||
            !almost_Equal(oldPos.y, pos.y) ||
            !almost_Equal(oldOrnt.value, ornt.value)) {

            car.distanceFromCamera = distanceFromCamera;

            etc.mng.markForNetSync(etc.id);
        }
    }

    void
    System_Car::postUpdate(EntityManager &mng,
                           duration32 dt) {}
}

