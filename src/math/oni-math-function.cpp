#include <oni-core/math/oni-math-function.h>

#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/math/oni-math-vec2.h>

namespace oni {
    namespace math {
        void
        findAABB(const component::Quad &quad,
                 component::AABB &aabb) {
            aabb.min = {math::min(math::min(quad.a.x, quad.b.x), math::min(quad.c.x, quad.d.x)),
                        math::min(math::min(quad.a.y, quad.b.y), math::min(quad.c.y, quad.d.y))};
            aabb.max = {math::max(math::max(quad.a.x, quad.b.x), math::max(quad.c.x, quad.d.x)),
                        math::max(math::max(quad.a.y, quad.b.y), math::max(quad.c.y, quad.d.y))};
        }

        math::vec2
        headingVector(common::r32 heading) {
            return vec2{std::cos(heading), std::sin(heading)};
        }

        math::vec2
        headingVector(const component::Heading &heading) {
            return headingVector(heading.value);
        }

        void
        translate(component::Quad &quad,
                  const component::WorldP3D &pos) {
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
        scale(component::Quad &quad,
              const component::Scale &scale) {
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
}