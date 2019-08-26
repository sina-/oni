#include <oni-core/math/oni-math-function.h>

#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/math/oni-math-vec2.h>


namespace oni {
    void
    findAABB(const Quad &quad,
             AABB &aabb) {
        aabb.min = {min(min(quad.a.x, quad.b.x), min(quad.c.x, quad.d.x)),
                    min(min(quad.a.y, quad.b.y), min(quad.c.y, quad.d.y))};
        aabb.max = {max(max(quad.a.x, quad.b.x), max(quad.c.x, quad.d.x)),
                    max(max(quad.a.y, quad.b.y), max(quad.c.y, quad.d.y))};
    }

    void
    translate(Quad &quad,
              const WorldP3D &pos) {
        quad.a.x += pos.x;
        quad.b.x += pos.x;
        quad.c.x += pos.x;
        quad.d.x += pos.x;

        quad.a.y += pos.y;
        quad.b.y += pos.y;
        quad.c.y += pos.y;
        quad.d.y += pos.y;
    }

    void
    translate(WorldP3D &pos,
              const WorldP3D &parent) {
        pos.x += parent.x;
        pos.y += parent.y;
    }

    void
    scale(Quad &quad,
          const Scale &scale) {
        quad.a.x *= scale.x;
        quad.b.x *= scale.x;
        quad.c.x *= scale.x;
        quad.d.x *= scale.x;

        quad.a.y *= scale.y;
        quad.b.y *= scale.y;
        quad.c.y *= scale.y;
        quad.d.y *= scale.y;
    }
}
