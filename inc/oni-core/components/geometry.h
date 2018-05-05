#pragma once

#include <oni-core/math/vec2.h>
#include <oni-core/math/vec3.h>
#include <oni-core/math/vec4.h>
#include <oni-core/math/mat4.h>
#include <oni-core/common/typedefs.h>

namespace oni {
    namespace components {

        struct Shape {
            /**
             *    NOTE: Local coordinates for dynamic objects and world coordinates for static objects.
             *
             *    B    C
             *    +----+
             *    |    |
             *    +----+
             *    A    D
             */
            math::vec3 vertexA{0.0f, 0.0f, 0.0f};
            math::vec3 vertexB{0.0f, 0.0f, 0.0f};
            math::vec3 vertexC{0.0f, 0.0f, 0.0f};
            math::vec3 vertexD{0.0f, 0.0f, 0.0f};

            math::vec3 getPosition() const { return vertexA; }

            math::vec2 getSize() {
                return math::vec2{vertexD.x - vertexA.x, vertexB.y - vertexA.y};
            }

            static Shape fromPositionAndSize(const math::vec3 &position, const math::vec2 &size) {
                return Shape{
                        math::vec3{position.x, position.y, position.z},
                        math::vec3{position.x, position.y + size.y, position.z},
                        math::vec3{position.x + size.x, position.y + size.y, position.z},
                        math::vec3{position.x + size.x, position.y, position.z}};
            }

            static Shape fromSizeAndRotation(const math::vec2 &size, const float rotation) {
                auto halfSizeX = size.x / 2;
                auto halfSizeY = size.y / 2;
                auto shape = Shape{
                        math::vec3{-halfSizeX, -halfSizeY, 0.0f},
                        math::vec3{-halfSizeX, halfSizeY, 0.0f},
                        math::vec3{halfSizeX, halfSizeY, 0.0f},
                        math::vec3{halfSizeX, -halfSizeY, 0.0f}};
                if (rotation) {
                    auto rotationMat = math::mat4::rotation(math::toRadians(rotation), math::vec3{0.0f, 0.0f, 1.0f});
                    shape.vertexA = rotationMat * shape.vertexA;
                    shape.vertexB = rotationMat * shape.vertexB;
                    shape.vertexC = rotationMat * shape.vertexC;
                    shape.vertexD = rotationMat * shape.vertexD;
                }
                return shape;
            }

        };

        struct Placement {
            math::vec3 position{0.0f, 0.0f, 0.0f};
            float rotation{0.0f};
            math::vec3 scale{1.0f, 1.0f, 0.0f};
        };

        struct TagStatic {
        };

        struct TagDynamic {
        };

        struct TagVehicle {
        };

        struct CarConfig {
            common::carSimDouble gravity{9.81f};
            common::carSimDouble mass{1200};
            common::carSimDouble inertialScale{1.0f};
            common::carSimDouble halfWidth{0.9f};
            common::carSimDouble cgToFront{2.0f}; // Distance from center of gravity to the front in meters.
            common::carSimDouble cgToRear{2.0f};
            common::carSimDouble cgToFrontAxle{1.25f};
            common::carSimDouble cgToRearAxle{1.25f};
            common::carSimDouble cgHeight{0.55f};
            common::carSimDouble wheelRadius{0.3f}; // For rendering only
            common::carSimDouble wheelWidth{0.2f}; // For rendering only
            common::carSimDouble tireGrip{2.0f};
            common::carSimDouble lockGrip{0.6f}; // % of grip when wheel is locked
            common::carSimDouble engineForce{4000.0f};
            common::carSimDouble brakeForce{12000.0f};
            common::carSimDouble eBrakeForce{12000.0f / 5.5f};
            common::carSimDouble weightTransfer{0.2f};
            common::carSimDouble maxSteer{0.6f}; // in radians
            common::carSimDouble cornerStiffnessFront{5.0f};
            common::carSimDouble cornerStiffnessRear{5.2f};
            common::carSimDouble airResist{2.5f};
            common::carSimDouble rollResist{8.0f};

            common::carSimDouble gearRatio{2.7f};
            common::carSimDouble differentialRatio{3.4f};
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

                position = math::vec2{0.0f, 0.0f};
                velocity = math::vec2{0.0f, 0.0f};
                velocityLocal = math::vec2{0.0f, 0.0f};
                acceleration = math::vec2{0.0f, 0.0f};
                accelerationLocal = math::vec2{0.0f, 0.0f};

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