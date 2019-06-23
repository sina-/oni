#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/entities/oni-entities-entity.h>
#include <oni-core/math/oni-math-vec3.h>
#include <cmath>

class b2Body;

namespace oni {
    namespace component {
        struct CarConfig {
            common::CarSimDouble gravity{9.81f};
            common::CarSimDouble mass{1200};
            common::CarSimDouble inertialScale{0.5f};
            common::CarSimDouble halfWidth{0.55f};
            common::CarSimDouble cgToFront{1.25f}; // Distance from center of gravity to the front in meters.
            common::CarSimDouble cgToRear{1.25f};
            common::CarSimDouble cgToFrontAxle{1.15f};
            common::CarSimDouble cgToRearAxle{1.f};
            common::CarSimDouble cgHeight{0.55f};
            common::CarSimDouble wheelRadius{0.25f}; // For rendering only
            common::CarSimDouble wheelWidth{0.2f}; // For rendering only
            common::CarSimDouble tireGrip{3.0f};
            common::CarSimDouble lockGrip{0.2f}; // % of grip when wheel is locked
            common::CarSimDouble engineForce{10000.f};
            common::CarSimDouble brakeForce{6000.f};
            common::CarSimDouble eBrakeForce{12000.f / 5.5f};
            common::CarSimDouble weightTransfer{0.2f};
            common::CarSimDouble maxSteer{0.5f}; // in radians
            common::CarSimDouble cornerStiffnessFront{5.0f};
            common::CarSimDouble cornerStiffnessRear{5.5f};
            common::CarSimDouble airResist{2.5f};
            common::CarSimDouble rollResist{8.0f};

            common::CarSimDouble gearRatio{2.7f};
            common::CarSimDouble differentialRatio{3.4f};
        };

        struct Car {
            common::CarSimDouble velocityAbsolute{0.f};
            common::CarSimDouble angularVelocity{0.f}; // Angular velocity in radians (rad/s)
            common::CarSimDouble steer{0.f}; // (-1.0..1.0)
            common::CarSimDouble steerAngle{0.f}; // (-maxSteer..maxSteer)
            common::CarSimDouble rpm{0.f};
            common::CarSimDouble accumulatedEBrake{0.f};
            common::CarSimDouble slipAngleFront{0.f};
            common::CarSimDouble slipAngleRear{0.f};

            common::CarSimDouble inertia{};
            common::CarSimDouble wheelBase{};
            common::CarSimDouble axleWeightRatioFront{};
            common::CarSimDouble axleWeightRatioRear{};
            common::CarSimDouble maxVelocityAbsolute{};

            math::vec2 velocity{0.f, 0.f}; // m/s
            math::vec2 velocityLocal{0.f, 0.f}; // m/s
            math::vec2 acceleration{0.f, 0.f};
            math::vec2 accelerationLocal{0.f, 0.f};

            bool accelerating{false};
            bool slippingFront{false};
            bool slippingRear{false};

            bool smoothSteer{true};
            bool safeSteer{true};

            common::CarSimDouble distanceFromCamera{1.f};

            bool isColliding{false};

            Car() = default;

            void
            applyConfiguration(const component::CarConfig &c) {
                inertia = c.mass * c.inertialScale;
                wheelBase = c.cgToFrontAxle + c.cgToRearAxle;
                axleWeightRatioFront = c.cgToRearAxle / wheelBase;
                axleWeightRatioRear = c.cgToFrontAxle / wheelBase;

                // Formula for solving quadratic equation: -c.airResistance*v^2 - c.rollResistance*v + c.engineForce = 0
                // Notice we are only interested in the positive answer.
                maxVelocityAbsolute = (c.rollResist -
                                       std::sqrt(c.rollResist * c.rollResist + 4 * c.airResist * c.engineForce)) /
                                      (-2 * c.airResist);
            }
        };

        enum class BodyType : common::u8 {
            UNKNOWN = 0,
            STATIC = 1,
            KINEMATIC = 2,
            DYNAMIC = 3,
        };

        enum class PhysicalCategory : common::u8 {
            UNKNOWN = 0,
            GENERIC = 1,
            VEHICLE = 2,
            RACE_CAR = 3,
            ROCKET = 4,
            WALL = 5,
        };

        struct Velocity {
            common::r32 currentVelocity{0.f};
            common::r32 maxVelocity{0.f};
        };

        struct Age {
            common::duration currentAge{0.f};
            common::duration maxAge{1.f};
        };

        union Force {
            struct {
                common::r32 x{0};
                common::r32 y{0};
            };
            math::vec2 value;
        };

        struct JetForce {
            common::r32 fuze{0}; // How long, in seconds, it takes for the fuel to burn
            common::r32 force{0};
        };

        struct PhysicalProperties {
            common::r32 linearDamping{2.f};
            common::r32 angularDamping{2.f};
            common::r32 density{0.1f};
            common::r32 friction{1.0f};
            bool highPrecision{false};
            bool colliding{false};
            bool collisionWithinCategory{false}; // Determines if instances of this object can collide with each other
            PhysicalCategory physicalCategory{PhysicalCategory::UNKNOWN};
            BodyType bodyType{BodyType::UNKNOWN};
        };
    }
}
