#pragma once

#include <components/component.h>

namespace oni {
    namespace entities {
        const static components::Mask Sprite = components::Mask().set(components::PLACEMENT).set(
                components::APPEARANCE);
        const static components::Mask DynamicSprite = components::Mask().set(components::PLACEMENT).set(
                components::APPEARANCE).set(components::VELOCITY);
        const static components::Mask TexturedSprite = components::Mask().set(components::PLACEMENT)
                .set(components::TEXTURE);
        const static components::Mask DynamicTexturedSprite = components::Mask().set(components::PLACEMENT)
                .set(components::TEXTURE).set(components::VELOCITY);
        const static components::Mask TextSprite = components::Mask().set(components::PLACEMENT)
                .set(components::TEXT);
    }
}
