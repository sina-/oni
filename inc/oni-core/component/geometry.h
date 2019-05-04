#pragma once

#include <oni-core/common/typedefs.h>
#include <oni-core/entities/entity.h>
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
    }
}