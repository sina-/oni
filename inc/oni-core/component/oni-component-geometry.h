#pragma once

#include <vector>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/entities/oni-entities-entity.h>
#include <oni-core/math/oni-math-vec2.h>
#include <oni-core/math/oni-math-vec3.h>
#include <oni-core/math/oni-math-vec4.h>
#include <oni-core/math/oni-math-mat4.h>

namespace oni {
    namespace component {
        union Point {
            math::vec3 value{};
            struct {
                common::r32 x;
                common::r32 y;
                common::r32 z;
            };
        };

        union WorldP3D {
            math::vec3 value{};
            struct {
                common::r32 x;
                common::r32 y;
                common::r32 z;
            };
        };

        union WorldP2D {
            math::vec2 value{};
            struct {
                common::r32 x;
                common::r32 y;
            };

            WorldP3D
            to3D(common::r32 z) const {
                return WorldP3D{value.x, value.y, z};
            }
        };

        union OriginP2D {
            math::vec2 value{};
            struct {
                common::r32 x;
                common::r32 y;
            };
        };

        union OriginP3D {
            math::vec3 value{};
            struct {
                common::r32 x;
                common::r32 y;
                common::r32 z;
            };
        };

        struct Heading {
            common::r32 value{0.f};
        };

        union Scale {
            math::vec3 value{1.f, 1.f, 1.f};
            struct {
                common::r32 x;
                common::r32 y;
                common::r32 z;
            };
        };

        struct EntityAttachment {
            std::vector<common::EntityID> entities;
            std::vector<entities::EntityType> entityTypes;
        };

        struct EntityAttachee {
            common::EntityID entityID;
            entities::EntityType entityType;
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
            math::vec3 vertexA{-0.5f, -0.5f, 0.f};
            math::vec3 vertexB{-0.5f, 0.5f, 0.f};
            math::vec3 vertexC{0.5f, 0.5f, 0.f};
            math::vec3 vertexD{0.5f, -0.5f, 0.f};

            math::vec3
            getPosition() const { return vertexA; }

            math::vec2
            getSize() const {
                return math::vec2{vertexD.x - vertexA.x, vertexB.y - vertexA.y};
            }

            void
            setSize(common::r32 x,
                    common::r32 y) {
                assert(x > 0);
                assert(y > 0);
                auto halfX = x / 2.f;
                auto halfY = y / 2.f;

                vertexA.x = -halfX;
                vertexA.y = -halfY;

                vertexB.x = -halfX;
                vertexB.y = halfY;

                vertexC.x = halfX;
                vertexC.y = halfY;

                vertexD.x = halfX;
                vertexD.y = -halfY;
            }

            void
            setSize(const math::vec2 &size) {
                setSize(size.x, size.y);
            }

            void
            centerAlign() {
                math::vec2 halfSize{};
                halfSize.x = (vertexD.x - vertexA.x) / 2;
                halfSize.y = (vertexB.y - vertexA.y) / 2;
                vertexA.x = -halfSize.x;
                vertexA.y = -halfSize.y;
                vertexB.x = -halfSize.x;
                vertexB.y = halfSize.y;
                vertexC.x = halfSize.x;
                vertexC.y = halfSize.y;
                vertexD.x = halfSize.x;
                vertexD.y = -halfSize.y;
            }

            void
            setZ(common::r32 z) {
                vertexA.z = z;
                vertexB.z = z;
                vertexC.z = z;
                vertexD.z = z;
            }

            static Shape
            // TODO: This is referenced from vertex A, but point of reference should be center of the sprite.
            fromPositionAndSize(const math::vec3 &position,
                                const math::vec2 &size) {
                return Shape{
                        math::vec3{position.x, position.y, position.z},
                        math::vec3{position.x, position.y + size.y, position.z},
                        math::vec3{position.x + size.x, position.y + size.y, position.z},
                        math::vec3{position.x + size.x, position.y, position.z}};
            }

            static Shape
            // TODO: This is referenced from vertex A, but point of reference should be center of the sprite.
            fromSizeAndRotation(const math::vec3 &size,
                                const common::r32 rotation) {
                auto shape = Shape{
                        {0,      0,      size.z},
                        {0,      size.y, size.z},
                        {size.x, size.y, size.z},
                        {size.x, 0,      size.z}
                };
                // Cast to ignore float imprecision.
                if (static_cast<common::u16>(rotation)) {
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
    }
}