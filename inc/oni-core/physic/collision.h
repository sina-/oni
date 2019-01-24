#pragma once

namespace oni {
    namespace component {
        class Shape;
    }

    namespace physic{
        bool collides(const component::Shape& first, const component::Shape& second);
    }
}
