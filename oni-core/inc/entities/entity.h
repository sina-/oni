#pragma once

#include <components/component.h>

namespace oni {
    namespace entities {

        typedef unsigned long entityID;

        const static components::Mask Sprite = components::Mask().set(components::PLACEMENT).set(
                components::APPEARANCE);
        const static components::Mask DynamicSprite = components::Mask().set(components::PLACEMENT).set(
                components::APPEARANCE).set(components::VELOCITY);
        const static components::Mask TexturedSprite = components::Mask().set(components::PLACEMENT)
                .set(components::TEXTURE);
        const static components::Mask DynamicTexturedSprite = components::Mask().set(components::PLACEMENT)
                .set(components::TEXTURE).set(components::VELOCITY);
        // TODO: add color
        const static components::Mask TextSprite = components::Mask().set(components::TEXT);
    }
}
