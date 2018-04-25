#pragma once

#include <oni-core/components/component.h>

namespace oni {
    namespace entities {

        const static components::Mask Sprite = components::Mask().set(components::PLACEMENT).set(
                components::APPEARANCE);
        const static components::Mask SpriteStatic = components::Mask().set(components::PLACEMENT).set(
                components::APPEARANCE).set(components::STATIC);
        const static components::Mask DynamicSprite = components::Mask().set(components::PLACEMENT).set(
                components::APPEARANCE).set(components::DYNAMIC);
        const static components::Mask TexturedSprite = components::Mask().set(components::PLACEMENT)
                .set(components::TEXTURE);
        const static components::Mask TexturedSpriteStatic = components::Mask().set(components::PLACEMENT)
                .set(components::TEXTURE).set(components::STATIC);
        const static components::Mask DynamicTexturedSprite = components::Mask().set(components::PLACEMENT)
                .set(components::TEXTURE).set(components::DYNAMIC);
        // TODO: add color
        const static components::Mask TextSprite = components::Mask().set(components::TEXT);
        const static components::Mask TextSpriteStatic = components::Mask().set(components::TEXT).set(
                components::STATIC);
    }
}
