#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/entities/oni-entities-structure.h>
#include <oni-core/math/oni-math-vec3.h>
#include <oni-core/math/oni-math-vec2.h>
#include <cmath>
#include <oni-core/util/oni-util-enum.h>


class b2Body;

class b2Contact;

namespace oni {
    struct CarConfig {
        r64 gravity{9.81f};
        r64 mass{1200};
        r64 inertialScale{0.5f};
        r64 halfWidth{0.55f};
        r64 cgToFront{1.25f}; // Distance from center of gravity to the front in meters.
        r64 cgToRear{1.25f};
        r64 cgToFrontAxle{1.15f};
        r64 cgToRearAxle{1.f};
        r64 cgHeight{0.55f};
        r64 wheelRadius{0.25f}; // For rendering only
        r64 wheelWidth{0.2f}; // For rendering only
        r64 tireGrip{3.0f};
        r64 lockGrip{0.2f}; // % of grip when wheel is locked
        r64 engineForce{10000.f};
        r64 brakeForce{6000.f};
        r64 eBrakeForce{12000.f / 5.5f};
        r64 weightTransfer{0.2f};
        r64 maxSteer{0.5f}; // in radians
        r64 cornerStiffnessFront{5.0f};
        r64 cornerStiffnessRear{5.5f};
        r64 airResist{2.5f};
        r64 rollResist{8.0f};

        r64 gearRatio{2.7f};
        r64 differentialRatio{3.4f};

        template<class Archive>
        void
        serialize(Archive &archive) {
            archive("gravity", gravity);
            archive("mass", mass);
            archive("inertialScale", inertialScale);
            archive("halfWidth", halfWidth);
            archive("cgToFront", cgToFront);
            archive("cgToRear", cgToRear);
            archive("cgToFrontAxle", cgToFrontAxle);
            archive("cgToRearAxle", cgToRearAxle);
            archive("cgHeight", cgHeight);
            archive("wheelRadius", wheelRadius);
            archive("wheelWidth", wheelWidth);
            archive("tireGrip", tireGrip);
            archive("lockGrip", lockGrip);
            archive("engineForce", engineForce);
            archive("brakeForce", brakeForce);
            archive("eBrakeForce", eBrakeForce);
            archive("weightTransfer", weightTransfer);
            archive("maxSteer", maxSteer);
            archive("cornerStiffnessFront", cornerStiffnessFront);
            archive("cornerStiffnessRear", cornerStiffnessRear);
            archive("airResist", airResist);
            archive("rollResist", rollResist);
            archive("gearRatio", gearRatio);
            archive("differentialRatio", differentialRatio);
        }
    };

    struct Car {
        r64 velocityAbsolute{0.f};
        r64 angularVelocity{0.f}; // Angular velocity in radians (rad/s)
        r64 steer{0.f}; // (-1.0..1.0)
        r64 steerAngle{0.f}; // (-maxSteer..maxSteer)
        r64 rpm{0.f};
        r64 slipAngleFront{0.f};
        r64 slipAngleRear{0.f};

        r64 inertia{};
        r64 wheelBase{};
        r64 axleWeightRatioFront{};
        r64 axleWeightRatioRear{};
        r64 maxVelocityAbsolute{};

        vec2 velocity{0.f, 0.f}; // m/s
        vec2 velocityLocal{0.f, 0.f}; // m/s
        vec2 acceleration{0.f, 0.f};
        vec2 accelerationLocal{0.f, 0.f};

        bool accelerating{false};
        bool slippingFront{false};
        bool slippingRear{false};

        bool smoothSteer{true};
        bool safeSteer{true};

        r64 distanceFromCamera{1.f};

        bool isColliding{false};

        Car() = default;

        void
        applyConfiguration(const CarConfig &c) {
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


        template<class Archive>
        void
        serialize(Archive &archive) {}
    };

    ONI_ENUM_DEF(BodyType, { 0, "Unknown" }, { 1, "Static" }, { 2, "Kinematic" }, { 3, "Dynamic" })

    ONI_ENUM_DEF(PhysicalCategory, { 0, "Unknown" }, { 1, "Vehicle" }, { 2, "RaceCar" }, { 3, "Rocket" }, { 4, "Wall" },
                 { 5, "Projectile" })

    struct PhysicalCatPair {
        PhysicalCategory a{};
        PhysicalCategory b{};
    };

    struct Velocity {
        r32 current{0.f};
        r32 max{0.f};

        template<class Archive>
        void
        serialize(Archive &archive) {
            archive(current, max);
        }
    };

    struct Acceleration {
        r32 current{0.f};
        r32 max{0.f};

        template<class Archive>
        void
        serialize(Archive &archive) {
            archive(current, max);
        }
    };

    union Velocity2D {
        vec2 value{};
        struct {
            r32 x;
            r32 y;
        };

        template<class Archive>
        void
        serialize(Archive &archive) {
            archive(x, y);
        }
    };

    union Acceleration2D {
        vec2 value{};
        struct {
            r32 x;
            r32 y;
        };

        template<class Archive>
        void
        serialize(Archive &archive) {
            archive(x, y);
        }
    };

    struct TimeToLive {
        duration32 currentAge{0.f};
        duration32 maxAge{1.f};

        template<class Archive>
        void
        serialize(Archive &archive) {
            archive(currentAge, maxAge);
        }
    };

    // TODO: Redundant
    union Force {
        vec2 value{};
        struct {
            r32 x;
            r32 y;
        };

        template<class Archive>
        void
        serialize(Archive &archive) {
            archive(x, y);
        }
    };

    union Force2D {
        vec2 value{};
        struct {
            r32 x;
            r32 y;
        };

        template<class Archive>
        void
        serialize(Archive &archive) {
            archive(x, y);
        }
    };

    struct Impulse2D {
        r32 value{};
        vec2 normal{};

        template<class Archive>
        void
        serialize(Archive &archive) {
            archive(value, normal);
        }
    };

    struct JetForce {
        r32 fuze{0}; // How long, in seconds, it takes for the fuel to burn
        r32 force{0};

        template<class Archive>
        void
        serialize(Archive &archive) {
            archive(fuze, force);
        }
    };

    struct PhysicalProperties {
        r32 linearDamping{2.f};
        r32 angularDamping{2.f};
        r32 density{0.1f};
        r32 friction{1.f};
        r32 gravityScale{1.f};
        bool highPrecision{false};
        bool collisionWithinCategory{false}; // Determines if instances of this object can collide with each other
        bool isSensor{false};
        BodyType bodyType{};
        PhysicalCategory physicalCategory{};

        bool colliding{false};

        template<class Archive>
        void
        serialize(Archive &archive) {
            archive("linearDamping", linearDamping);
            archive("angularDamping", angularDamping);
            archive("density", density);
            archive("friction", friction);
            archive("gravityScale", gravityScale);
            archive("highPrecision", highPrecision);
            archive("collisionWithinCategory", collisionWithinCategory);
            archive("isSensor", isSensor);
            archive("bodyType", bodyType);
            archive("physicalCategory", physicalCategory);
        }
    };

    struct PhysicalBody {
        b2Body *value;
    };
}
