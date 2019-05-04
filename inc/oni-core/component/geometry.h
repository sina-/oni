#pragma once

#include <oni-core/common/typedefs.h>
#include <oni-core/entities/entity.h>
#include <oni-core/math/vec2.h>
#include <oni-core/math/vec3.h>
#include <oni-core/math/vec4.h>
#include <oni-core/math/mat4.h>
#include <oni-core/math/transformation.h>

#define DEFINE_ACCESSORS_3D auto &x() { return value.x; } auto &y() { return value.y; } auto &z() { return value.z; } \
                            const auto&x() const{ return value.x;} const auto&y() const{ return value.y;} const auto&z() const{ return value.y;}
#define DEFINE_ACCESSORS_2D auto &x() { return value.x; } auto &y() { return value.y; } \
                            const auto&x() const{ return value.x;} const auto&y() const{ return value.y;}

namespace oni {
    namespace component {
        struct Point {
            math::vec3 value{};
        };

        struct WorldP3D {
            math::vec3 value{};
        };

        struct WorldP2D {
            math::vec2 value{};

            WorldP3D
            to3D(common::real32 z) const {
                return WorldP3D{value.x, value.y, z};
            }
        };

        struct Heading {
            common::real32 value{0.f};
        };

        struct Scale {
            math::vec3 value{1.f, 1.f, 1.f};
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
            component::WorldP3D vertexA{0.f, 0.f, 1.f};
            component::WorldP3D vertexB{0.f, 1.f, 1.f};
            component::WorldP3D vertexC{1.f, 1.f, 1.f};
            component::WorldP3D vertexD{1.f, 0.f, 1.f};

            component::WorldP3D
            getPosition() const { return vertexA; }

            math::vec2
            getSize() const {
                return math::vec2{vertexD.value.x - vertexA.value.x, vertexB.value.y - vertexA.value.y};
            }

            void
            setSizeFromOrigin(const math::vec2 &size) {
                vertexB.value.y = size.y;
                vertexC.value.x = size.x;
                vertexC.value.y = size.y;
                vertexD.value.x = size.x;
            }

            void
            moveToWorldCoordinates(const WorldP3D &worldPos) {
                math::localToWorldTranslation(worldPos, *this);
            }

            void
            centerAlign() {
                math::vec2 halfSize{};
                halfSize.x = (vertexD.value.x - vertexA.value.x) / 2;
                halfSize.y = (vertexB.value.y - vertexA.value.y) / 2;
                vertexA.value.x -= halfSize.x;
                vertexA.value.y -= halfSize.y;
                vertexB.value.x -= halfSize.x;
                vertexB.value.y -= halfSize.y;
                vertexC.value.x -= halfSize.x;
                vertexC.value.y -= halfSize.y;
                vertexD.value.x -= halfSize.x;
                vertexD.value.y -= halfSize.y;
            }

            void
            setZ(common::real32 z) {
                vertexA.value.z = z;
                vertexB.value.z = z;
                vertexC.value.z = z;
                vertexD.value.z = z;
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
                        component::WorldP3D{0, 0, size.z},
                        component::WorldP3D{0, size.y, size.z},
                        component::WorldP3D{size.x, size.y, size.z},
                        component::WorldP3D{size.x, 0, size.z}
                };
                // Cast to ignore float imprecision.
                if (static_cast<common::uint16>(rotation)) {
                    auto halfSize = math::vec3{size.x / 2.0f, size.y / 2.0f, size.z};

                    shape.vertexA.value -= halfSize;
                    shape.vertexB.value -= halfSize;
                    shape.vertexC.value -= halfSize;
                    shape.vertexD.value -= halfSize;

                    auto rotationMat = math::mat4::rotation(math::toRadians(rotation), math::vec3{0.f, 0.f, 1.0f});
                    shape.vertexA.value = rotationMat * shape.vertexA.value;
                    shape.vertexB.value = rotationMat * shape.vertexB.value;
                    shape.vertexC.value = rotationMat * shape.vertexC.value;
                    shape.vertexD.value = rotationMat * shape.vertexD.value;

                    shape.vertexA.value += halfSize;
                    shape.vertexB.value += halfSize;
                    shape.vertexC.value += halfSize;
                    shape.vertexD.value += halfSize;
                }
                return shape;
            }
        };
    }
}