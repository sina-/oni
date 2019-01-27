#pragma once

namespace oni {
    namespace component {
        class Shape;
    }

    namespace physics{
        bool collides(const component::Shape& first, const component::Shape& second);
    }
}
