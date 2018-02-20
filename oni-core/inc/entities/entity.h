#pragma once

#include <components/component.h>

namespace oni {
    namespace entities {
        const static components::Mask Sprite = components::Mask().set(components::POSITION).set(components::APPEARANCE);
    }
}
