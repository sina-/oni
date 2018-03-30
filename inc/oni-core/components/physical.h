#pragma once

#include <oni-core/math/vec2.h>
#include <oni-core/math/vec3.h>
#include <oni-core/math/vec4.h>
#include <oni-core/math/mat4.h>

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
        };

        struct Velocity {
            math::vec3 direction;
            float magnitude;

            Velocity() : direction(math::vec3()), magnitude(0.0f) {};

            Velocity(const math::vec3 &_direction, float _magnitude) : direction(_direction),
                                                                       magnitude(_magnitude) {};

            Velocity(const Velocity &other) = default;

        };

        // TODO: better name
        typedef double carSimDouble;

        struct CarConfig {
            carSimDouble gravity;
            carSimDouble mass;
            carSimDouble inertialScale;
            carSimDouble halfWidth;
            carSimDouble cgToFront;
            carSimDouble cgToRear;
            carSimDouble cgToFrontAxle;
            carSimDouble cgToRearAxle;
            carSimDouble cgHeight;
            carSimDouble wheelRadius;
            carSimDouble wheelWidth;
            carSimDouble tireGrip;
            carSimDouble lockGrip;
            carSimDouble engineForce;
            carSimDouble brakeForce;
            carSimDouble eBrakeForce;
            carSimDouble weightTransfer;
            carSimDouble maxSteer;
            carSimDouble cornerStiffnessFront;
            carSimDouble cornerStiffnessRear;
            carSimDouble airResist;
            carSimDouble rollResist;
            carSimDouble scaleMultiplierX;
            carSimDouble scaleMultiplierY;

            // TODO: move this to cpp file
            CarConfig() {
                gravity = 9.81f;
                engineForce = 3000.0f;
                brakeForce = 12000.0f;
                mass = 1200.0f;
                inertialScale = 1.0f;
                halfWidth = 0.8f;
                cgToFront = 2.0f;
                cgToRear = 2.0f;
                cgToFrontAxle = 1.25f;
                cgToRearAxle = 1.25f;
                cgHeight = 0.55f;
                wheelRadius = 0.3f;
                wheelWidth = 0.2f;
                tireGrip = 2.0f;
                lockGrip = 0.6f;
                eBrakeForce = brakeForce / 2.5f;
                weightTransfer = 0.2f;
                maxSteer = 0.9f;
                cornerStiffnessFront = 5.0f;
                cornerStiffnessRear = 5.2f;
                airResist = 2.5f;
                rollResist = 8.0f;
                scaleMultiplierX = 0.25f;
                scaleMultiplierY = 0.30f;
            }
        };

        struct Car {
            carSimDouble heading;
            carSimDouble velocityAbsolute;
            carSimDouble yawRate;
            carSimDouble steer;
            carSimDouble steerAngle;
            carSimDouble inertia;
            carSimDouble wheelBase;
            carSimDouble axleWeightRatioFront;
            carSimDouble axleWeightRatioRear;

            math::vec2 position;
            math::vec2 velocity;
            math::vec2 velocityLocal;
            math::vec2 acceleration;
            math::vec2 accelerationLocal;

            bool smoothSteer;
            bool safeSteer;

            explicit Car(const components::CarConfig &carConfig) {
                heading = 0.0f;
                velocityAbsolute = 0.0f;
                yawRate = 0.0f;
                steer = 0.0f;
                steerAngle = 0.0f;
                inertia = carConfig.mass * carConfig.inertialScale;
                wheelBase = carConfig.cgToFrontAxle + carConfig.cgToRearAxle;
                axleWeightRatioFront = carConfig.cgToRearAxle / wheelBase;
                axleWeightRatioRear = carConfig.cgToFrontAxle / wheelBase;

                position = math::vec2();
                velocity = math::vec2();
                velocityLocal = math::vec2();
                acceleration = math::vec2();
                accelerationLocal = math::vec2();

                smoothSteer = true;
                safeSteer = true;
            }
        };


    }
}