#pragma once

#include <oni-core/math/vec2.h>
#include <oni-core/math/vec3.h>
#include <oni-core/math/vec4.h>
#include <oni-core/math/mat4.h>
#include <oni-core/common/typedefs.h>

// TODO: This file name sucks

namespace oni {
    namespace components {
        struct Placement {
            /**
             *    B    C
             *    +----+
             *    |    |
             *    +----+
             *    A    D
             */
            math::vec3 vertexA;
            math::vec3 vertexB;
            math::vec3 vertexC;
            math::vec3 vertexD;

            Placement() : vertexA(math::vec3()), vertexB(math::vec3()),
                          vertexC(math::vec3()), vertexD(math::vec3()) {}

            Placement(const math::vec3 &mPositionA, const math::vec3 &mPositionB,
                      const math::vec3 &mPositionC, const math::vec3 &mPositionD) :
                    vertexA(mPositionA),
                    vertexB(mPositionB),
                    vertexC(mPositionC),
                    vertexD(mPositionD) {}

            Placement(const math::vec3 position, const math::vec2 size) {
                vertexA = math::vec3(position.x, position.y, position.z);
                vertexB = math::vec3(position.x, position.y + size.y, position.z);
                vertexC = math::vec3(position.x + size.x, position.y + size.y, position.z);
                vertexD = math::vec3(position.x + size.x, position.y, position.z);
            }

            Placement(const Placement &other) = default;

            math::vec3 getPosition() const { return vertexA; }
        };

        struct CarConfig {
            common::carSimDouble gravity;
            common::carSimDouble mass;
            common::carSimDouble inertialScale;
            common::carSimDouble halfWidth;
            common::carSimDouble cgToFront; // Distance from center of gravity to the front in meters.
            common::carSimDouble cgToRear;
            common::carSimDouble cgToFrontAxle;
            common::carSimDouble cgToRearAxle;
            common::carSimDouble cgHeight;
            common::carSimDouble wheelRadius; // For rendering only
            common::carSimDouble wheelWidth; // For rendering only
            common::carSimDouble tireGrip;
            common::carSimDouble lockGrip; // % of grip when wheel is locked
            common::carSimDouble engineForce;
            common::carSimDouble brakeForce;
            common::carSimDouble eBrakeForce;
            common::carSimDouble weightTransfer;
            common::carSimDouble maxSteer; // in radians
            common::carSimDouble cornerStiffnessFront;
            common::carSimDouble cornerStiffnessRear;
            common::carSimDouble airResist;
            common::carSimDouble rollResist;

            //carSimDouble scaleMultiplierX; // Car image scale along X multiplier for rendering
            //carSimDouble scaleMultiplierY; // Car image scale along Y multiplier for rendering

            common::carSimDouble gearRatio;
            common::carSimDouble differentialRatio;

            CarConfig() {
                gravity = 9.81f;
                engineForce = 4000.0f;
                brakeForce = 12000.0f;
                mass = 1200.0f;
                inertialScale = 1.0f;
                halfWidth = 0.9f;
                cgToFront = 2.0f;
                cgToRear = 2.0f;
                cgToFrontAxle = 1.25f;
                cgToRearAxle = 1.25f;
                cgHeight = 0.55f;
                wheelRadius = 0.3f;
                wheelWidth = 0.2f;
                tireGrip = 2.0f;
                lockGrip = 0.6f;
                eBrakeForce = brakeForce / 5.5f;
                weightTransfer = 0.2f;
                maxSteer = 0.6f;
                cornerStiffnessFront = 5.0f;
                cornerStiffnessRear = 5.2f;
                airResist = 2.5f;
                rollResist = 8.0f;
                //scaleMultiplierX = 0.25f;
                //scaleMultiplierY = 0.30f;
                gearRatio = 2.7f;
                differentialRatio = 3.4f;
            }
        };

        struct Car {
            common::carSimDouble heading;
            common::carSimDouble velocityAbsolute;
            common::carSimDouble angularVelocity; // Angular velocity in radians (rad/s)
            common::carSimDouble steer; // (-1.0..1.0)
            common::carSimDouble steerAngle; // (-maxSteer..maxSteer)
            common::carSimDouble inertia;
            common::carSimDouble wheelBase;
            common::carSimDouble axleWeightRatioFront;
            common::carSimDouble axleWeightRatioRear;
            common::carSimDouble rpm;
            common::carSimDouble maxVelocityAbsolute;
            common::carSimDouble accumulatedEBrake;
            common::carSimDouble slipAngleFront;
            common::carSimDouble slipAngleRear;


            math::vec2 position;
            math::vec2 velocity; // m/s
            math::vec2 velocityLocal; // m/s
            math::vec2 acceleration;
            math::vec2 accelerationLocal;

            bool accelerating;
            bool slippingFront;
            bool slippingRear;

            bool smoothSteer;
            bool safeSteer;

            explicit Car(const components::CarConfig &c) {
                heading = 0.0f;
                velocityAbsolute = 0.0f;
                angularVelocity = 0.0f;
                steer = 0.0f;
                steerAngle = 0.0f;
                inertia = c.mass * c.inertialScale;
                wheelBase = c.cgToFrontAxle + c.cgToRearAxle;
                axleWeightRatioFront = c.cgToRearAxle / wheelBase;
                axleWeightRatioRear = c.cgToFrontAxle / wheelBase;
                slipAngleFront = 0.0f;
                slipAngleRear = 0.0f;

                position = math::vec2();
                velocity = math::vec2();
                velocityLocal = math::vec2();
                acceleration = math::vec2();
                accelerationLocal = math::vec2();

                rpm = 0.0f;
                accelerating = false;
                // Formula for solving quadratic equation: -c.airResistance*v^2 - c.rollResistance*v + c.engineForce = 0
                // Notice we are only interested in the positive answer.
                maxVelocityAbsolute = (c.rollResist -
                                       std::sqrt(c.rollResist * c.rollResist + 4 * c.airResist * c.engineForce)) /
                                      (-2 * c.airResist);
                accumulatedEBrake = 0.0f;

                smoothSteer = true;
                safeSteer = true;
                slippingFront = false;
                slippingRear = false;
            }
        };


    }
}