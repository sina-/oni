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

        struct Rectangle {
            math::vec3 A{-1.f, -1.f};
            math::vec3 B{-1.f, 1.f};
            math::vec3 C{1.f, 1.f};
            math::vec3 D{1.f, -1.f};

            static Rectangle
            // TODO: This is referenced from vertex A, but point of reference should be center of the sprite.
            fromPositionAndSize(common::r32 posX,
                                common::r32 posY,
                                common::r32 sizeX,
                                common::r32 sizeY) {
                return Rectangle{
                        math::vec3{posX, posY},
                        math::vec3{posX, posY + sizeY},
                        math::vec3{posX + sizeX, posY + sizeY},
                        math::vec3{posX + sizeX, posY}};
            }

            math::vec2
            getSize() const {
                return math::vec2{D.x - A.x, B.y - A.y};
            }
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
    }
}