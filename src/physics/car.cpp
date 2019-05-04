#include <oni-core/physics/car.h>
#include <oni-core/component/input-data.h>
#include <oni-core/component/physic.h>
#include <oni-core/math/functions.h>


namespace oni {
    namespace physics {
        void
        tickCar(component::Car &car,
                const component::CarConfig &config,
                const component::CarInput &inputs,
                common::real64 dt) {
            using common::CarSimDouble;
            CarSimDouble sn = std::sin(car.heading);
            CarSimDouble cs = std::cos(car.heading);

            car.velocityLocal.x = cs * car.velocity.x + sn * car.velocity.y;
            car.velocityLocal.y = cs * car.velocity.y - sn * car.velocity.x;

            CarSimDouble axleWeightFront = config.mass * (car.axleWeightRatioFront * config.gravity -
                                                          config.weightTransfer * car.accelerationLocal.x *
                                                          config.cgHeight /
                                                          car.wheelBase);
            CarSimDouble axleWeightRear = config.mass * (car.axleWeightRatioRear * config.gravity +
                                                         config.weightTransfer * car.accelerationLocal.x *
                                                         config.cgHeight /
                                                         car.wheelBase);

            // Resulting velocity of the wheels as result of the yaw rate of the car body.
            // v = angularVelocity * r where r is distance from axle to CG and angularVelocity (angular velocity) in rad/s.
            CarSimDouble yawSpeedFront = config.cgToFrontAxle * car.angularVelocity;
            CarSimDouble yawSpeedRear = -config.cgToRearAxle * car.angularVelocity;

            // Calculate slip angles for front and rear wheels (a.k.a. alpha)
            CarSimDouble slipAngleFront =
                    std::atan2(car.velocityLocal.y + yawSpeedFront, std::abs(car.velocityLocal.x)) -
                    math::sign(car.velocityLocal.x) * car.steerAngle;
            CarSimDouble slipAngleRear = std::atan2(car.velocityLocal.y + yawSpeedRear,
                                                    std::abs(car.velocityLocal.x));

            car.slippingFront = std::abs(slipAngleFront) > 0.6f;
            car.slippingRear = std::abs(slipAngleRear) > 0.6f;

            car.slipAngleFront = slipAngleFront;
            car.slipAngleRear = slipAngleRear;

            CarSimDouble tireGripFront = config.tireGrip;
            CarSimDouble tireGripRear = config.tireGrip * (1.0 - inputs.eBrake *
                                                                 (1.0 -
                                                                  config.lockGrip)); // reduce rear grip when eBrake is on

            CarSimDouble frictionForceFrontLocalY =
                    math::clip(-config.cornerStiffnessFront * slipAngleFront, -tireGripFront, tireGripFront) *
                    axleWeightFront;
            CarSimDouble frictionForceRearLocalY =
                    math::clip(-config.cornerStiffnessRear * slipAngleRear, -tireGripRear, tireGripRear) *
                    axleWeightRear;

            //  Get amount of brake/throttle from our inputs
            CarSimDouble brake = std::min(inputs.brake * config.brakeForce + inputs.eBrake * config.eBrakeForce,
                                          config.brakeForce);
            // TODO: The actual formula for this has to include current gear and RPM:
            // http://www.asawicki.info/Mirror/Car%20Physics%20for%20Games/Car%20Physics%20for%20Games.html
            CarSimDouble throttle = inputs.throttle * config.engineForce;

            //  Resulting force in local car coordinates.
            //  This is implemented as a Rear-Wheel-Drive car only.
            CarSimDouble tractionForceLocalX = throttle - brake * math::sign(car.velocityLocal.x);
            CarSimDouble tractionForceLocalY = 0;

            CarSimDouble rollingResistanceForceLocalX = -config.rollResist * car.velocityLocal.x;
            CarSimDouble rollingResistanceForceLocalY = -config.rollResist * car.velocityLocal.y;

            CarSimDouble dragForceLocalX = -config.airResist * car.velocityLocal.x * std::abs(car.velocityLocal.x);
            CarSimDouble dragForceLocalY = -config.airResist * car.velocityLocal.y * std::abs(car.velocityLocal.y);

            // total force in car coordinates
            CarSimDouble totalForceLocalX = rollingResistanceForceLocalX + dragForceLocalX + tractionForceLocalX;
            CarSimDouble totalForceLocalY = rollingResistanceForceLocalY + dragForceLocalY + tractionForceLocalY +
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
            CarSimDouble wheelAngularVelocity = car.velocityAbsolute / config.wheelRadius;
            // TODO: This is incorrect in case of slipping.
            car.rpm = wheelAngularVelocity * config.gearRatio * config.differentialRatio * 60 / (2 * M_PI);
            if (car.rpm < 400.0f) {
                car.rpm = 400.0f;
            }

            // calculate rotational forces
            CarSimDouble angularTorque = (frictionForceFrontLocalY + tractionForceLocalY) * config.cgToFrontAxle -
                                         frictionForceRearLocalY * config.cgToRearAxle;

            //  Sim gets unstable at very slow speeds, so just stop the car
            if (std::abs(car.velocityAbsolute) < 0.5 && !throttle) {
                car.velocity.x = car.velocity.y = car.velocityAbsolute = 0;
                angularTorque = car.angularVelocity = 0;
            }

            CarSimDouble angularAcceleration = angularTorque / car.inertia;

            car.angularVelocity += angularAcceleration * dt;
            car.heading += car.angularVelocity * dt;

            //  finally we can update position
            car.position.x += car.velocity.x * dt;
            car.position.y += car.velocity.y * dt;
        }

        common::CarSimDouble
        applySmoothSteer(const component::Car &car,
                         common::CarSimDouble steerInput,
                         common::real64 dt) {
            common::CarSimDouble steer = 0;

            if (std::abs(steerInput) > 0.001) {
                //  Move toward steering input
                steer = math::clip(car.steer + steerInput * dt * 2.0, -1.0, 1.0); // -inp.right, inp.left);
            } else {
                //  No steer input - move toward centre (0)
                if (car.steer > 0) {
                    steer = std::max(car.steer - dt * 1.0f, static_cast<common::CarSimDouble> (0.0f));
                } else if (car.steer < 0) {
                    steer = std::min(car.steer + dt * 1.0f, static_cast<common::CarSimDouble> (0.0f));
                }
            }

            return steer;
        }

        common::CarSimDouble
        applySafeSteer(const component::Car &car,
                       common::CarSimDouble steerInput) {
            auto avel = std::min(car.velocityAbsolute, 250.0);
            auto steer = steerInput * (1.0f - (avel / 280.0));
            return steer;
        }
    }
}