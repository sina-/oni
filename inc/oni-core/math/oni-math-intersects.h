#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/math/oni-math-vec3.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-fwd.h>


namespace oni {
    bool
    intersects(const Rectangle &first,
               const Rectangle &second);

    bool
    intersects(const Rectangle &first,
               const Point &second);

    bool
    intersects(const Rectangle &first,
               r32 x,
               r32 y,
               r32 lengthX,
               r32 lengthY);

    bool
    intersects(const WorldP3D &first,
               r32 x,
               r32 y,
               r32 lengthX,
               r32 lengthY);

    bool
    intersects(const WorldP3D &,
               const Scale &,
               r32 x,
               r32 y,
               r32 lengthX,
               r32 lengthY);

    bool
    intersects(const Point &first,
               r32 x,
               r32 y,
               r32 lengthX,
               r32 lengthY);
}
