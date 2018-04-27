#pragma once

#include <oni-core/components/component.h>

namespace oni {
    namespace entities {

        const static auto Sprite = components::Mask().set(components::PLACEMENT).set(
                components::APPEARANCE);
        const static auto SpriteStatic = components::Mask().set(components::PLACEMENT).set(
                components::APPEARANCE).set(components::STATIC);
        const static auto SpriteDynamic = components::Mask().set(components::PLACEMENT).set(
                components::APPEARANCE).set(components::VEHICLE_DYNAMICS);
        const static auto SpriteTextured = components::Mask().set(components::PLACEMENT)
                .set(components::TEXTURE);
        const static auto SpriteTexturedStatic = components::Mask().set(components::PLACEMENT)
                .set(components::TEXTURE).set(components::STATIC);
        const static auto SpriteTexturedDynamic = components::Mask().set(components::PLACEMENT)
                .set(components::TEXTURE).set(components::DYNAMIC);
        const static auto VehicleTextured = components::Mask().set(components::PLACEMENT)
                .set(components::TEXTURE).set(components::VEHICLE_DYNAMICS);
        // TODO: add color
        const static auto SpriteText = components::Mask().set(components::TEXT);
        const static auto SpriteTextStatic = components::Mask().set(components::TEXT).set(components::STATIC);

        bool isSprite(const components::Mask &mask);

        bool isSpriteStatic(const components::Mask &mask);

        bool isSpriteDynamic(const components::Mask &mask);

        bool isSpriteTextured(const components::Mask &mask);

        bool isSpriteTexturedStatic(const components::Mask &mask);

        bool isSpriteTexturedDynamic(const components::Mask &mask);

        bool isVehicleTextured(const components::Mask &mask);

        bool isSpriteText(const components::Mask &mask);

        bool isSpriteTextStatic(const components::Mask &mask);
    }
}
