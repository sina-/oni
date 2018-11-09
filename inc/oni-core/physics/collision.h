#pragma once

namespace oni {
    namespace components {
        class Shape;
    }

    namespace physics{
        bool collides(const components::Shape& first, const components::Shape& second);
    }
}
