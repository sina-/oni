#pragma once

#include <oni-core/common/typedefs.h>
#include <oni-core/math/vec3.h>

namespace oni {
    namespace component {
        class Shape;

        using Point = math::vec3;
    }

    namespace math {
        bool collides(const component::Shape &first, const component::Shape &second);

        bool collides(const component::Shape &first, const component::Point &second);

        bool collides(const component::Shape &first,
                      common::real32 x, common::real32 y,
                      common::real32 lengthX, common::real32 lengthY);

        bool collides(const component::Point &first,
                      common::real32 x, common::real32 y,
                      common::real32 lengthX, common::real32 lengthY);
    }
}
