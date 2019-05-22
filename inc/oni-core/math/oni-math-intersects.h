#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/math/oni-math-vec3.h>
#include <oni-core/component/oni-component-geometry.h>

namespace oni {
    namespace component {
        struct Rectangle;
        union WorldP3D;
        union Point;
    }

    namespace math {
        bool
        intersects(const component::Rectangle &first,
                   const component::Rectangle &second);

        bool
        intersects(const component::Rectangle &first,
                   const component::Point &second);

        bool
        intersects(const component::Rectangle &first,
                   common::r32 x,
                   common::r32 y,
                   common::r32 lengthX,
                   common::r32 lengthY);

        bool
        intersects(const component::WorldP3D &first,
                   common::r32 x,
                   common::r32 y,
                   common::r32 lengthX,
                   common::r32 lengthY);

        bool
        intersects(const component::WorldP3D &,
                   const component::Scale &,
                   common::r32 x,
                   common::r32 y,
                   common::r32 lengthX,
                   common::r32 lengthY);

        bool
        intersects(const component::Point &first,
                   common::r32 x,
                   common::r32 y,
                   common::r32 lengthX,
                   common::r32 lengthY);
    }
}
