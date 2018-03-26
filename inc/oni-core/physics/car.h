#pragma once

#include <cmath>
#include <algorithm>

#include <oni-core/math/vec2.h>
#include <oni-core/components/input.h>

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

        static void tick(components::Car &car, const components::CarConfig &config,
                         const components::CarInput &inputs, float dt) {
            using components::carScalar;

            carScalar sn = std::sin(car.heading);
            carScalar cs = std::cos(car.heading);

            car.velocityLocal.x = cs * car.velocity.x + sn * car.velocity.y;
            car.velocityLocal.y = cs * car.velocity.y - sn * car.velocity.x;

            carScalar axleWeightFront = config.mass * (car.axleWeightRatioFront * config.gravity -
                                                       config.weightTransfer * car.accelerationLocal.x *
                                                       config.cgHeight /
                                                       car.wheelBase);
            carScalar axleWeightRear = config.mass * (car.axleWeightRatioRear * config.gravity +
                                                      config.weightTransfer * car.accelerationLocal.x *
                                                      config.cgHeight /
                                                      car.wheelBase);

            // Resulting velocity of the wheels as result of the yaw rate of the car body.
            // v = yawRate * r where r is distance from axle to CG and yawRate (angular velocity) in rad/s.
            carScalar yawSpeedFront = config.cgToFrontAxle * car.yawRate;
            carScalar yawSpeedRear = -config.cgToRearAxle * car.yawRate;

            // Calculate slip angles for front and rear wheels (a.k.a. alpha)
            carScalar slipAngleFront = std::atan2(car.velocityLocal.y + yawSpeedFront, std::abs(car.velocityLocal.x)) -
                                       sign(car.velocityLocal.x) * car.steerAngle;
            carScalar slipAngleRear = std::atan2(car.velocityLocal.y + yawSpeedRear, std::abs(car.velocityLocal.x));

            carScalar tireGripFront = config.tireGrip;
            carScalar tireGripRear = config.tireGrip *
                                     (1.0 -
                                      inputs.eBrake * (1.0 - config.lockGrip)); // reduce rear grip when eBrake is on

            carScalar frictionForceFront_cy =
                    clip(-config.cornerStiffnessFront * slipAngleFront, -tireGripFront, tireGripFront) *
                    axleWeightFront;
            carScalar frictionForceRear_cy =
                    clip(-config.cornerStiffnessRear * slipAngleRear, -tireGripRear, tireGripRear) * axleWeightRear;

            //  Get amount of brake/throttle from our inputs
            carScalar brake = std::min(inputs.brake * config.brakeForce + inputs.eBrake * config.eBrakeForce,
                                       config.brakeForce);
            carScalar throttle = inputs.throttle * config.engineForce;

            //  Resulting force in local car coordinates.
            //  This is implemented as a RWD car only.
            carScalar tractionForce_cx = throttle - brake * sign(car.velocityLocal.x);
            carScalar tractionForce_cy = 0;

            carScalar dragForce_cx = -config.rollResist * car.velocityLocal.x -
                                     config.airResist * car.velocityLocal.x * std::abs(car.velocityLocal.x);
            carScalar dragForce_cy = -config.rollResist * car.velocityLocal.y -
                                     config.airResist * car.velocityLocal.y * std::abs(car.velocityLocal.y);

            // total force in car coordinates
            carScalar totalForce_cx = dragForce_cx + tractionForce_cx;
            carScalar totalForce_cy =
                    dragForce_cy + tractionForce_cy + std::cos(car.steerAngle) * frictionForceFront_cy +
                    frictionForceRear_cy;

            // acceleration along car axes
            car.accelerationLocal.x = totalForce_cx / config.mass;  // forward/reverse acceleration
            car.accelerationLocal.y = totalForce_cy / config.mass;  // sideways acceleration

            // acceleration in world coordinates
            car.acceleration.x = cs * car.accelerationLocal.x - sn * car.accelerationLocal.y;
            car.acceleration.y = sn * car.accelerationLocal.x + cs * car.accelerationLocal.y;

            // update velocity
            car.velocity.x += car.acceleration.x * dt;
            car.velocity.y += car.acceleration.y * dt;

            car.velocityAbsolute = car.velocity.len();

            // calculate rotational forces
            carScalar angularTorque = (frictionForceFront_cy + tractionForce_cy) * config.cgToFrontAxle -
                                      frictionForceRear_cy * config.cgToRearAxle;

            //  Sim gets unstable at very slow speeds, so just stop the car
            if (std::abs(car.velocityAbsolute) < 0.5 && !throttle) {
                car.velocity.x = car.velocity.y = car.velocityAbsolute = 0;
                angularTorque = car.yawRate = 0;
            }

            carScalar angularAcceleration = angularTorque / car.inertia;

            car.yawRate += angularAcceleration * dt;
            car.heading += car.yawRate * dt;

            //  finally we can update position
            car.position.x += car.velocity.x * dt;
            car.position.y += car.velocity.y * dt;

        }

    }
}
