#pragma once

#include <oni-core/common/typedefs.h>
#include <oni-core/math/vec3.h>

namespace oni {
    namespace component {
        struct Shape;
        struct WorldP3D;
        struct Point;
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
