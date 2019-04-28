#pragma once

#include <oni-core/common/typedefs.h>
#include <oni-core/component/entity.h>
#include <oni-core/math/vec2.h>
#include <oni-core/math/vec3.h>
#include <oni-core/math/vec4.h>
#include <oni-core/math/mat4.h>
#include <oni-core/math/transformation.h>

namespace oni {
    namespace component {
        using Point = math::vec3;

        struct EntityAttachment {
            std::vector<common::EntityID> entities;
            std::vector<component::EntityType> entityTypes;
        };

        struct EntityAttachee {
            common::EntityID entityID;
            component::EntityType entityType;
        };

        // TODO: Rename this into Sprite
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
            math::vec3 vertexA{0.f, 0.f, 1.f};
            math::vec3 vertexB{0.f, 1.f, 1.f};
            math::vec3 vertexC{1.f, 1.f, 1.f};
            math::vec3 vertexD{1.f, 0.f, 1.f};

            math::vec3
            getPosition() const { return vertexA; }

            math::vec2
            getSize() const {
                return math::vec2{vertexD.x - vertexA.x, vertexB.y - vertexA.y};
            }

            void
            setSizeFromOrigin(const math::vec2 &size) {
                vertexB.y = size.y;
                vertexC.x = size.x;
                vertexC.y = size.y;
                vertexD.x = size.x;
            }

            void
            moveToWorldCoordinates(const math::vec3 &worldPos) {
                math::Transformation::localToWorldTranslation(worldPos, *this);
            }

            void
            centerAlign() {
                math::vec2 halfSize{};
                halfSize.x = (vertexD.x - vertexA.x) / 2;
                halfSize.y = (vertexB.y - vertexA.y) / 2;
                vertexA.x -= halfSize.x;
                vertexA.y -= halfSize.y;
                vertexB.x -= halfSize.x;
                vertexB.y -= halfSize.y;
                vertexC.x -= halfSize.x;
                vertexC.y -= halfSize.y;
                vertexD.x -= halfSize.x;
                vertexD.y -= halfSize.y;
            }

            void
            setZ(common::real32 z) {
                vertexA.z = z;
                vertexB.z = z;
                vertexC.z = z;
                vertexD.z = z;
            }

            static Shape
            fromPositionAndSize(const math::vec3 &position,
                                const math::vec2 &size) {
                return Shape{
                        math::vec3{position.x, position.y, position.z},
                        math::vec3{position.x, position.y + size.y, position.z},
                        math::vec3{position.x + size.x, position.y + size.y, position.z},
                        math::vec3{position.x + size.x, position.y, position.z}};
            }

            static Shape
            fromSizeAndRotation(const math::vec3 &size,
                                const common::real32 rotation) {
                auto shape = Shape{
                        math::vec3{0, 0, size.z},
                        math::vec3{0, size.y, size.z},
                        math::vec3{size.x, size.y, size.z},
                        math::vec3{size.x, 0, size.z}
                };
                // Cast to ignore float imprecision.
                if (static_cast<common::uint16>(rotation)) {
                    auto halfSize = math::vec3{size.x / 2.0f, size.y / 2.0f, size.z};

                    shape.vertexA -= halfSize;
                    shape.vertexB -= halfSize;
                    shape.vertexC -= halfSize;
                    shape.vertexD -= halfSize;

                    auto rotationMat = math::mat4::rotation(math::toRadians(rotation), math::vec3{0.f, 0.f, 1.0f});
                    shape.vertexA = rotationMat * shape.vertexA;
                    shape.vertexB = rotationMat * shape.vertexB;
                    shape.vertexC = rotationMat * shape.vertexC;
                    shape.vertexD = rotationMat * shape.vertexD;

                    shape.vertexA += halfSize;
                    shape.vertexB += halfSize;
                    shape.vertexC += halfSize;
                    shape.vertexD += halfSize;
                }
                return shape;
            }

        };

        struct Placement {
            math::vec3 position{0.f, 0.f, 0.f};
            common::real32 rotation{0.f}; // In radians
            math::vec3 scale{1.0f, 1.0f, 0.f};
        };

        struct ChunkIndex {
            common::int64 x{0};
            common::int64 y{0};
        };

        struct RoadTileIndex {
            // NOTE: This is relative to Chunk
            common::uint16 x{0};
            common::uint16 y{0};
        };

        struct TileIndex {
            common::int64 x{0};
            common::int64 y{0};
        };

        struct EdgeRoadTile {
            RoadTileIndex eastBoarder{};
            RoadTileIndex southBoarder{};
            RoadTileIndex westBoarder{};
            RoadTileIndex northBoarder{};
        };

        struct Chunk {
            math::vec3 position{0.f, 0.f, 0.f};
            common::uint64 index{0};
            EdgeRoadTile edgeRoad{};
        };

        // TODO: Why the shit is this here?
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
            common::CarSimDouble heading{0.f}; // In radians
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


            math::vec2 position{0.f, 0.f};
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
    }
}