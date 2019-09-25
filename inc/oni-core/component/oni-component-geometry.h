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
    union Point {
        vec3 value{};
        struct {
            r32 x;
            r32 y;
            r32 z;
        };
    };

    struct Rectangle {
        vec3 A{-1.f, -1.f};
        vec3 B{-1.f, 1.f};
        vec3 C{1.f, 1.f};
        vec3 D{1.f, -1.f};

        static Rectangle
        // TODO: This is referenced from vertex A, but point of reference should be center of the sprite.
        fromPositionAndSize(r32 posX,
                            r32 posY,
                            r32 sizeX,
                            r32 sizeY) {
            return Rectangle{
                    vec3{posX, posY},
                    vec3{posX, posY + sizeY},
                    vec3{posX + sizeX, posY + sizeY},
                    vec3{posX + sizeX, posY}};
        }

        vec2
        getSize() const {
            return vec2{D.x - A.x, B.y - A.y};
        }
    };

    union WorldP3D {
        vec3 value{};
        struct {
            r32 x;
            r32 y;
            r32 z;
        };
    };

    union WorldP4D {
        vec4 value{};
        struct {
            r32 x;
            r32 y;
            r32 z;
            r32 w;
        };
    };

    union WorldP2D {
        vec2 value{};
        struct {
            r32 x;
            r32 y;
        };

        WorldP3D
        to3D(r32 z) const {
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

        u16 size{1};
        std::deque<WorldP3D> pos{};
    };

    union OriginP2D {
        vec2 value{};
        struct {
            r32 x;
            r32 y;
        };
    };

    union OriginP3D {
        vec3 value{};
        struct {
            r32 x;
            r32 y;
            r32 z;
        };
    };

    struct Orientation {
        r32 value{0.f}; // radians
    };

    union Direction {
        vec2 value{};
        struct {
            r32 x;
            r32 y;
        };
    };

    union Scale {
        vec3 value{1.f, 1.f, 1.f};
        struct {
            r32 x;
            r32 y;
            r32 z;
        };
    };

    struct EntityAttachment {
        std::vector<EntityID> entities;
    };

    struct EntityAttachee {
        EntityID entityID;
    };

    struct Quad {
        /**
         *    b +---+ c
         *      |  /|
         *      |/  |
         *    a +---+ d
         **/
        WorldP3D a{-0.5f, -0.5f, +1};
        WorldP3D b{-0.5f, +0.5f, +1};
        WorldP3D c{+0.5f, +0.5f, +1};
        WorldP3D d{+0.5f, -0.5f, +1};

        static inline Quad
        make(const WorldP3D &pos,
             const Scale &scale) {
            auto halfSizeX = scale.x / 2.f;
            auto halfSizeY = scale.y / 2.f;
            return {{pos.x - halfSizeX, pos.y - halfSizeY, pos.z},
                    {pos.x - halfSizeX, pos.y + halfSizeY, pos.z},
                    {pos.x + halfSizeX, pos.y + halfSizeY, pos.z},
                    {pos.x + halfSizeX, pos.y - halfSizeY, pos.z}};
        }

        static inline Quad
        make(const WorldP3D &pos,
             const Orientation &ornt,
             const Scale &scale) {
            // NOTE: Very slow obviously, so use sparingly, ideally only for debugging

            auto translationMat = mat4::translation(pos.value);
            auto rotationMat = mat4::rotation(ornt.value, vec3{0.0f, 0.0f, 1.0f});
            auto scaleMat = mat4::scale(scale.value);
            auto trans = translationMat * rotationMat * scaleMat;

            auto halfSizeX = 0.5f;
            auto halfSizeY = 0.5f;
            auto quad = Quad{
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
        setZ(r32 z) {
            a.z = z;
            b.z = z;
            c.z = z;
            d.z = z;
        }
    };

    struct AABB {
        vec2 min{};
        vec2 max{};

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
            return vec2{min.x, max.y};
        }

        auto
        bottomLeft() {
            return vec2{min.x, min.y};
        }

        auto
        bottomRight() {
            return vec2{max.x, min.y};
        }

        auto
        topRight() {
            return vec2{max.x, max.y};
        }
    };
}
