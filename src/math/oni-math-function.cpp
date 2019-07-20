#include <oni-core/math/oni-math-function.h>
#include <oni-core/component/oni-component-geometry.h>

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
    }
}