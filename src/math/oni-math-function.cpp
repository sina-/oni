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
        math::headingVector(common::r32 heading) {
            return vec2{std::cos(heading), std::sin(heading)};
        }

        math::vec2
        headingVector(const component::Heading &heading) {
            return headingVector(heading.value);
        }
    }
}