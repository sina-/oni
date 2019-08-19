#include <oni-core/math/oni-math-intersects.h>


namespace oni {
    bool
    intersects(const Rectangle &first,
               const Rectangle &second) {
        // TODO: Obvious optimization is to store the center in Shape instead of recalculation every tick.

        auto firstCenterX = (first.A.x + first.C.x) / 2.0f;
        auto secondCenterX = (second.A.x + second.C.x) / 2.0f;

        auto firstCenterY = (first.A.y + first.C.y) / 2.0f;
        auto secondCenterY = (second.A.y + second.C.y) / 2.0f;

        // TODO: Maybe store size in Shape?
        auto firstSize = first.getSize();
        auto secondSize = second.getSize();

        return (abs(firstCenterX - secondCenterX) * 2 < (firstSize.x + secondSize.x)) &&
               (abs(firstCenterY - secondCenterY) * 2 < (firstSize.y + secondSize.y));
    }

    bool
    intersects(const Rectangle &first,
               const Point &second) {
        auto firstCenterX = (first.A.x + first.C.x) / 2.0f;
        auto firstCenterY = (first.A.y + first.C.y) / 2.0f;
        auto firstSize = first.getSize();

        return (abs(firstCenterX - second.x) * 2 < (firstSize.x)) &&
               (abs(firstCenterY - second.y) * 2 < (firstSize.y));
    }

    bool
    intersects(const Rectangle &first,
               r32 x,
               r32 y,
               r32 lengthX,
               r32 lengthY) {
        auto firstCenterX = (first.A.x + first.C.x) / 2.0f;
        auto firstCenterY = (first.A.y + first.C.y) / 2.0f;
        auto firstSize = first.getSize();

        return (abs(firstCenterX - x) * 2 < (firstSize.x + lengthX)) &&
               (abs(firstCenterY - y) * 2 < (firstSize.y + lengthY));
    }

    bool
    intersects(const Point &first,
               r32 x,
               r32 y,
               r32 lengthX,
               r32 lengthY) {
        return (abs(first.x - x) * 2 < lengthX) &&
               (abs(first.y - y) * 2 < lengthY);
    }

    bool
    intersects(const WorldP3D &first,
               r32 x,
               r32 y,
               r32 lengthX,
               r32 lengthY) {
        return (abs(first.x - x) * 2 < lengthX) &&
               (abs(first.y - y) * 2 < lengthY);
    }

    bool
    intersects(const WorldP3D &pos,
               const Scale &scale,
               r32 x,
               r32 y,
               r32 lengthX,
               r32 lengthY) {
        return (abs(pos.x - x) * 2 < lengthX + scale.x) &&
               (abs(pos.y - y) * 2 < lengthY + scale.y);
    }
}
