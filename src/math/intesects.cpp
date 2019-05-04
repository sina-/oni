#include <oni-core/math/intesects.h>

#include <oni-core/component/geometry.h>
#include <oni-core/common/typedefs-graphics.h>

namespace oni {
    namespace math {
        bool
        intersects(const component::Shape &first,
                   const component::Shape &second) {
            // TODO: Obvious optimization is to store the center in Shape instead of recalculation every tick.

            auto firstCenterX = (first.vertexA.value.x + first.vertexC.value.x) / 2.0f;
            auto secondCenterX = (second.vertexA.value.x + second.vertexC.value.x) / 2.0f;

            auto firstCenterY = (first.vertexA.value.y + first.vertexC.value.y) / 2.0f;
            auto secondCenterY = (second.vertexA.value.y + second.vertexC.value.y) / 2.0f;

            // TODO: Maybe store size in Shape?
            auto firstSize = first.getSize();
            auto secondSize = second.getSize();

            return (std::abs(firstCenterX - secondCenterX) * 2 < (firstSize.x + secondSize.x)) &&
                   (std::abs(firstCenterY - secondCenterY) * 2 < (firstSize.y + secondSize.y));
        }

        bool
        intersects(const component::Shape &first,
                   const component::Point &second) {
            auto firstCenterX = (first.vertexA.value.x + first.vertexC.value.x) / 2.0f;
            auto firstCenterY = (first.vertexA.value.y + first.vertexC.value.y) / 2.0f;
            auto firstSize = first.getSize();

            return (std::abs(firstCenterX - second.value.x) * 2 < (firstSize.x)) &&
                   (std::abs(firstCenterY - second.value.y) * 2 < (firstSize.y));
        }

        bool
        intersects(const component::Shape &first,
                   common::real32 x,
                   common::real32 y,
                   common::real32 lengthX,
                   common::real32 lengthY) {
            auto firstCenterX = (first.vertexA.value.x + first.vertexC.value.x) / 2.0f;
            auto firstCenterY = (first.vertexA.value.y + first.vertexC.value.y) / 2.0f;
            auto firstSize = first.getSize();

            return (std::abs(firstCenterX - x) * 2 < (firstSize.x + lengthX)) &&
                   (std::abs(firstCenterY - y) * 2 < (firstSize.y + lengthY));
        }

        bool
        intersects(const component::Point &first,
                   common::real32 x,
                   common::real32 y,
                   common::real32 lengthX,
                   common::real32 lengthY) {
            return (std::abs(first.value.x - x) * 2 < lengthX) &&
                   (std::abs(first.value.y - y) * 2 < lengthY);
        }

        bool
        intersects(const component::WorldP3D &first,
                   common::real32 x,
                   common::real32 y,
                   common::real32 lengthX,
                   common::real32 lengthY) {
            return (std::abs(first.value.x - x) * 2 < lengthX) &&
                   (std::abs(first.value.y - y) * 2 < lengthY);
        }
    }
}