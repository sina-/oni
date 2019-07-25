#pragma once

#include <deque>
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

        union WorldP4D {
            math::vec4 value{};
            struct {
                common::r32 x;
                common::r32 y;
                common::r32 z;
                common::r32 w;
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

        struct WorldP3D_History {
            void
            add(const WorldP3D &p) {
                if (!pos.empty() && (pos.back().value - p.value).len() < 1.0f) {
                    return;
                }
                if (pos.size() + 1 > size) {
                    pos.pop_front();
                }
                pos.push_back(p);
            }

            common::u16 size{1};
            std::deque<WorldP3D> pos{};
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
            common::r32 value{0.f}; // radians
        };

        union Heading2D {
            math::vec2 value{};
            struct {
                common::r32 x;
                common::r32 y;
            };
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

        struct Quad {
            /**
             *    b +---+ c
             *      |  /|
             *      |/  |
             *    a +---+ d
             **/
            component::WorldP3D a{-0.5f, -0.5f, +1};
            component::WorldP3D b{-0.5f, +0.5f, +1};
            component::WorldP3D c{+0.5f, +0.5f, +1};
            component::WorldP3D d{+0.5f, -0.5f, +1};

            static inline Quad
            make(const component::WorldP3D &pos,
                 const component::Scale &scale) {
                auto halfSizeX = scale.x / 2.f;
                auto halfSizeY = scale.y / 2.f;
                return {{pos.x - halfSizeX, pos.y - halfSizeY, pos.z},
                        {pos.x - halfSizeX, pos.y + halfSizeY, pos.z},
                        {pos.x + halfSizeX, pos.y + halfSizeY, pos.z},
                        {pos.x + halfSizeX, pos.y - halfSizeY, pos.z}};
            }

            static inline Quad
            make(const component::WorldP3D &pos,
                 const component::Heading &heading,
                 const component::Scale &scale) {
                // NOTE: Very slow obviously, so use sparingly, ideally only for debugging

                auto translationMat = math::mat4::translation(pos.value);
                auto rotationMat = math::mat4::rotation(heading.value, math::vec3{0.0f, 0.0f, 1.0f});
                auto scaleMat = math::mat4::scale(scale.value);
                auto trans = translationMat * rotationMat * scaleMat;

                auto halfSizeX = 0.5f;
                auto halfSizeY = 0.5f;
                auto quad = component::Quad{
                        {-halfSizeX, -halfSizeY, pos.z},
                        {-halfSizeX, +halfSizeY, pos.z},
                        {+halfSizeX, +halfSizeY, pos.z},
                        {+halfSizeX, -halfSizeY, pos.z},
                };
                quad.a.value = trans * quad.a.value;
                quad.b.value = trans * quad.b.value;
                quad.c.value = trans * quad.c.value;
                quad.d.value = trans * quad.d.value;
                return quad;
            }

            void
            setZ(common::r32 z) {
                a.z = z;
                b.z = z;
                c.z = z;
                d.z = z;
            }
        };

        struct AABB {
            math::vec2 min{};
            math::vec2 max{};

            auto
            width() {
                return max.x - min.x;
            }

            auto
            height() {
                return max.y - min.y;
            }

            auto
            topLeft() {
                return math::vec2{min.x, max.y};
            }

            auto
            bottomLeft() {
                return math::vec2{min.x, min.y};
            }

            auto
            bottomRight() {
                return math::vec2{max.x, min.y};
            }

            auto
            topRight() {
                return math::vec2{max.x, max.y};
            }
        };
    }
}