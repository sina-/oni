#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/math/oni-math-vec3.h>
#include <oni-core/component/oni-component-geometry.h>

namespace oni {
    namespace component {
        struct Shape;
        union WorldP3D;
        union Point;
    }

    namespace math {
        bool
        intersects(const component::Shape &first,
                   const component::Shape &second);

        bool
        intersects(const component::Shape &first,
                   const component::Point &second);

        bool
        intersects(const component::Shape &first,
                   common::real32 x,
                   common::real32 y,
                   common::real32 lengthX,
                   common::real32 lengthY);

        bool
        intersects(const component::WorldP3D &first,
                   common::real32 x,
                   common::real32 y,
                   common::real32 lengthX,
                   common::real32 lengthY);

        bool
        intersects(const component::Point &first,
                   common::real32 x,
                   common::real32 y,
                   common::real32 lengthX,
                   common::real32 lengthY);
    }
}
