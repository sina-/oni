#include <oni-core/entities/entity.h>

namespace oni {
    namespace entities {

        bool isSprite(const components::Mask &mask) {
            return (mask & Sprite) == Sprite;
        }

        bool isSpriteStatic(const components::Mask &mask) {
            return (mask & SpriteStatic) == SpriteStatic;
        }

        bool isSpriteDynamic(const components::Mask &mask) {
            return (mask & SpriteDynamic) == SpriteDynamic;
        }

        bool isSpriteTextured(const components::Mask &mask) {
            return (mask & SpriteTextured) == SpriteTextured;
        }

        bool isSpriteTexturedStatic(const components::Mask &mask) {
            return (mask & SpriteTexturedStatic) == SpriteTexturedStatic;
        }

        bool isSpriteTexturedDynamic(const components::Mask &mask) {
            return (mask & SpriteTexturedDynamic) == SpriteTexturedDynamic;
        }

        bool isVehicleTextured(const components::Mask &mask) {
            return (mask & VehicleTextured) == VehicleTextured;
        }

        bool isSpriteText(const components::Mask &mask) {
            return (mask & SpriteText) == SpriteText;
        }

        bool isSpriteTextStatic(const components::Mask &mask) {
            return (mask & SpriteTextStatic) == SpriteTextStatic;
        }
    }
}