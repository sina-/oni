#include <oni-core/physics/collision.h>

#include <oni-core/component/geometry.h>

namespace oni {
    namespace physics {

        bool collides(const component::Shape &first, const component::Shape &second) {
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
    }
}