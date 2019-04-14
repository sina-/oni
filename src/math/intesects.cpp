#include <oni-core/math/intesects.h>

#include <oni-core/component/geometry.h>
#include <oni-core/common/typedefs-graphics.h>

namespace oni {
    namespace math {
        bool
        intersects(const component::Shape &first,
                   const component::Shape &second) {
            // TODO: Obvious optimization is to store the center in Shape instead of recalculation every tick.

            auto firstCenterX = (first.vertexA.x + first.vertexC.x) / 2.0f;
            auto secondCenterX = (second.vertexA.x + second.vertexC.x) / 2.0f;

            auto firstCenterY = (first.vertexA.y + first.vertexC.y) / 2.0f;
            auto secondCenterY = (second.vertexA.y + second.vertexC.y) / 2.0f;

            // TODO: Maybe store size in Shape?
            auto firstSize = first.getSize();
            auto secondSize = second.getSize();

            return (std::abs(firstCenterX - secondCenterX) * 2 < (firstSize.x + secondSize.x)) &&
                   (std::abs(firstCenterY - secondCenterY) * 2 < (firstSize.y + secondSize.y));
        }

        bool
        intersects(const component::Shape &first,
                   const component::Point &second) {
            auto firstCenterX = (first.vertexA.x + first.vertexC.x) / 2.0f;
            auto firstCenterY = (first.vertexA.y + first.vertexC.y) / 2.0f;
            auto firstSize = first.getSize();

            return (std::abs(firstCenterX - second.x) * 2 < (firstSize.x)) &&
                   (std::abs(firstCenterY - second.y) * 2 < (firstSize.y));
        }

        bool
        intersects(const component::Shape &first,
                   common::real32 x,
                   common::real32 y,
                   common::real32 lengthX,
                   common::real32 lengthY) {
            auto firstCenterX = (first.vertexA.x + first.vertexC.x) / 2.0f;
            auto firstCenterY = (first.vertexA.y + first.vertexC.y) / 2.0f;
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
            return (std::abs(first.x - x) * 2 < lengthX) &&
                   (std::abs(first.y - y) * 2 < lengthY);
        }
    }
}