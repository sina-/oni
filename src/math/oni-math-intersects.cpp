#include <oni-core/math/oni-math-intersects.h>

#include <oni-core/common/oni-common-typedefs-graphic.h>

namespace oni {
    namespace math {
        bool
        intersects(const component::Rectangle &first,
                   const component::Rectangle &second) {
            // TODO: Obvious optimization is to store the center in Shape instead of recalculation every tick.

            auto firstCenterX = (first.A.x + first.C.x) / 2.0f;
            auto secondCenterX = (second.A.x + second.C.x) / 2.0f;

            auto firstCenterY = (first.A.y + first.C.y) / 2.0f;
            auto secondCenterY = (second.A.y + second.C.y) / 2.0f;

            // TODO: Maybe store size in Shape?
            auto firstSize = first.getSize();
            auto secondSize = second.getSize();

            return (math::abs(firstCenterX - secondCenterX) * 2 < (firstSize.x + secondSize.x)) &&
                   (math::abs(firstCenterY - secondCenterY) * 2 < (firstSize.y + secondSize.y));
        }

        bool
        intersects(const component::Rectangle &first,
                   const component::Point &second) {
            auto firstCenterX = (first.A.x + first.C.x) / 2.0f;
            auto firstCenterY = (first.A.y + first.C.y) / 2.0f;
            auto firstSize = first.getSize();

            return (math::abs(firstCenterX - second.x) * 2 < (firstSize.x)) &&
                   (math::abs(firstCenterY - second.y) * 2 < (firstSize.y));
        }

        bool
        intersects(const component::Rectangle &first,
                   common::r32 x,
                   common::r32 y,
                   common::r32 lengthX,
                   common::r32 lengthY) {
            auto firstCenterX = (first.A.x + first.C.x) / 2.0f;
            auto firstCenterY = (first.A.y + first.C.y) / 2.0f;
            auto firstSize = first.getSize();

            return (math::abs(firstCenterX - x) * 2 < (firstSize.x + lengthX)) &&
                   (math::abs(firstCenterY - y) * 2 < (firstSize.y + lengthY));
        }

        bool
        intersects(const component::Point &first,
                   common::r32 x,
                   common::r32 y,
                   common::r32 lengthX,
                   common::r32 lengthY) {
            return (math::abs(first.x - x) * 2 < lengthX) &&
                   (math::abs(first.y - y) * 2 < lengthY);
        }

        bool
        intersects(const component::WorldP3D &first,
                   common::r32 x,
                   common::r32 y,
                   common::r32 lengthX,
                   common::r32 lengthY) {
            return (math::abs(first.x - x) * 2 < lengthX) &&
                   (math::abs(first.y - y) * 2 < lengthY);
        }

        bool
        intersects(const component::WorldP3D &pos,
                   const component::Scale &scale,
                   common::r32 x,
                   common::r32 y,
                   common::r32 lengthX,
                   common::r32 lengthY) {
            return (math::abs(pos.x - x) * 2 < lengthX + scale.x) &&
                   (math::abs(pos.y - y) * 2 < lengthY + scale.y);
        }
    }
}