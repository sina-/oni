#include <oni-core/physics/oni-physics-car.h>
#include <oni-core/io/oni-io-input-data.h>
#include <oni-core/component/oni-component-physics.h>
#include <oni-core/math/oni-math-function.h>


namespace oni {
    void
    tickCar(Car &car,
            WorldP3D &pos,
            Heading &heading,
            const CarConfig &config,
            const CarInput &inputs,
            r64 dt) {
        r64 sn = std::sin(heading.value);
        r64 cs = std::cos(heading.value);

        car.velocityLocal.x = cs * car.velocity.x + sn * car.velocity.y;
        car.velocityLocal.y = cs * car.velocity.y - sn * car.velocity.x;

        r64 axleWeightFront = config.mass * (car.axleWeightRatioFront * config.gravity -
                                             config.weightTransfer * car.accelerationLocal.x *
                                             config.cgHeight /
                                             car.wheelBase);
        r64 axleWeightRear = config.mass * (car.axleWeightRatioRear * config.gravity +
                                            config.weightTransfer * car.accelerationLocal.x *
                                            config.cgHeight /
                                            car.wheelBase);

        // Resulting velocity of the wheels as result of the yaw rate of the car body.
        // v = angularVelocity * r where r is distance from axle to CG and angularVelocity (angular velocity) in rad/s.
        r64 yawSpeedFront = config.cgToFrontAxle * car.angularVelocity;
        r64 yawSpeedRear = -config.cgToRearAxle * car.angularVelocity;

        // Calculate slip angles for front and rear wheels (a.k.a. alpha)
        r64 slipAngleFront =
                std::atan2(car.velocityLocal.y + yawSpeedFront, std::abs(car.velocityLocal.x)) -
                sign(car.velocityLocal.x) * car.steerAngle;
        r64 slipAngleRear = std::atan2(car.velocityLocal.y + yawSpeedRear,
                                       std::abs(car.velocityLocal.x));

        car.slippingFront = std::abs(slipAngleFront) > 0.6f;
        car.slippingRear = std::abs(slipAngleRear) > 0.6f;

        car.slipAngleFront = slipAngleFront;
        car.slipAngleRear = slipAngleRear;

        r64 tireGripFront = config.tireGrip;
        r64 tireGripRear = config.tireGrip * (1.0 - inputs.eBrake *
                                                    (1.0 -
                                                     config.lockGrip)); // reduce rear grip when eBrake is on

        r64 frictionForceFrontLocalY =
                clip(-config.cornerStiffnessFront * slipAngleFront, -tireGripFront, tireGripFront) *
                axleWeightFront;
        r64 frictionForceRearLocalY =
                clip(-config.cornerStiffnessRear * slipAngleRear, -tireGripRear, tireGripRear) *
                axleWeightRear;

        //  Get amount of brake/throttle from our inputs
        r64 brake = std::min(inputs.brake * config.brakeForce + inputs.eBrake * config.eBrakeForce,
                             config.brakeForce);
        // TODO: The actual formula for this has to include current gear and RPM:
        // http://www.asawicki.info/Mirror/Car%20Physics%20for%20Games/Car%20Physics%20for%20Games.html
        r64 throttle = inputs.throttle * config.engineForce;

        //  Resulting force in local car coordinates.
        //  This is implemented as a Rear-Wheel-Drive car only.
        r64 tractionForceLocalX = throttle - brake * sign(car.velocityLocal.x);
        r64 tractionForceLocalY = 0;

        r64 rollingResistanceForceLocalX = -config.rollResist * car.velocityLocal.x;
        r64 rollingResistanceForceLocalY = -config.rollResist * car.velocityLocal.y;

        r64 dragForceLocalX = -config.airResist * car.velocityLocal.x * std::abs(car.velocityLocal.x);
        r64 dragForceLocalY = -config.airResist * car.velocityLocal.y * std::abs(car.velocityLocal.y);

        // total force in car coordinates
        r64 totalForceLocalX = rollingResistanceForceLocalX + dragForceLocalX + tractionForceLocalX;
        r64 totalForceLocalY = rollingResistanceForceLocalY + dragForceLocalY + tractionForceLocalY +
                               std::cos(car.steerAngle) * frictionForceFrontLocalY +
                               frictionForceRearLocalY;

        // acceleration along car axes
        car.accelerationLocal.x = totalForceLocalX / config.mass;  // forward/reverse acceleration
        car.accelerationLocal.y = totalForceLocalY / config.mass;  // sideways acceleration

        // acceleration in world coordinates
        car.acceleration.x = cs * car.accelerationLocal.x - sn * car.accelerationLocal.y;
        car.acceleration.y = sn * car.accelerationLocal.x + cs * car.accelerationLocal.y;

        // update velocity
        car.velocity.x += car.acceleration.x * dt;
        car.velocity.y += car.acceleration.y * dt;

        auto velocityAbsolute = car.velocity.len();
        car.accelerating = velocityAbsolute > car.velocityAbsolute;
        car.velocityAbsolute = velocityAbsolute;
        r64 wheelAngularVelocity = car.velocityAbsolute / config.wheelRadius;
        // TODO: This is incorrect in case of slipping.
        car.rpm = wheelAngularVelocity * config.gearRatio * config.differentialRatio * 60 / (2 * M_PI);
        if (car.rpm < 400.0f) {
            car.rpm = 400.0f;
        }

        // calculate rotational forces
        r64 angularTorque = (frictionForceFrontLocalY + tractionForceLocalY) * config.cgToFrontAxle -
                            frictionForceRearLocalY * config.cgToRearAxle;

        //  Sim gets unstable at very slow speeds, so just stop the car
        if (std::abs(car.velocityAbsolute) < 0.5 && !throttle) {
            car.velocity.x = car.velocity.y = car.velocityAbsolute = 0;
            angularTorque = car.angularVelocity = 0;
        }

        r64 angularAcceleration = angularTorque / car.inertia;

        car.angularVelocity += angularAcceleration * dt;

        heading.value += car.angularVelocity * dt;
        pos.x += car.velocity.x * dt;
        pos.y += car.velocity.y * dt;
    }

    r64
    applySmoothSteer(const Car &car,
                     r64 steerInput,
                     r64 dt) {
        r64 steer = 0;

        if (std::abs(steerInput) > 0.001) {
            //  Move toward steering input
            steer = clip(car.steer + steerInput * dt * 2.0, -1.0, 1.0); // -inp.right, inp.left);
        } else {
            //  No steer input - move toward centre (0)
            if (car.steer > 0) {
                steer = std::max(car.steer - dt * 1.0f, static_cast<r64> (0.0f));
            } else if (car.steer < 0) {
                steer = std::min(car.steer + dt * 1.0f, static_cast<r64> (0.0f));
            }
        }

        return steer;
    }

    r64
    applySafeSteer(const Car &car,
                   r64 steerInput) {
        auto avel = std::min(car.velocityAbsolute, 250.0);
        auto steer = steerInput * (1.0f - (avel / 280.0));
        return steer;
    }
}
