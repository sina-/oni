#include <oni-core/entities/create-entity.h>

namespace oni {
    namespace entities {

        entities::entityID createSpriteEntity(BasicEntityRepo &basicEntityRepo, components::LayerID layerID,
                                              const math::vec4 &color,
                                              const math::vec3 &position, const math::vec2 &size) {
            auto entity = basicEntityRepo.createEntity(entities::Sprite, layerID);

            auto entityPlacement = components::Placement(position, size);
            auto entityAppearance = components::Appearance(color);

            basicEntityRepo.setEntityPlacement(entity, entityPlacement);
            basicEntityRepo.setEntityAppearance(entity, entityAppearance);

            return entity;
        }

        entities::entityID createTexturedEntity(BasicEntityRepo &basicEntityRepo, components::LayerID layerID,
                                                const components::Texture &entityTexture,
                                                const math::vec3 &position, const math::vec2 &size) {
            auto entity = basicEntityRepo.createEntity(entities::TexturedSprite, layerID);

            auto entityPlacement = components::Placement(position, size);

            basicEntityRepo.setEntityPlacement(entity, entityPlacement);
            basicEntityRepo.setEntityTexture(entity, entityTexture);

            return entity;
        }

        entities::entityID createDynamicTexturedEntity(BasicEntityRepo &basicEntityRepo, components::LayerID layerID,
                                                       const components::Texture &entityTexture,
                                                       const math::vec3 &position, const math::vec2 &size) {
            auto entity = basicEntityRepo.createEntity(entities::DynamicTexturedSprite, layerID);

            auto entityPlacement = components::Placement(position, size);

            auto entityVelocity = components::Velocity();
            entityVelocity.magnitude = 8.0f;
            entityVelocity.direction = math::vec3(0.0f, 0.0f, 0.0f);

            basicEntityRepo.setEntityPlacement(entity, entityPlacement);
            basicEntityRepo.setEntityVelocity(entity, entityVelocity);
            basicEntityRepo.setEntityTexture(entity, entityTexture);

            return entity;
        }

        entities::entityID createTextEntity(BasicEntityRepo &basicEntityRepo, graphics::FontManager &fontManager,
                                            components::LayerID layerID, const std::string &text,
                                            const math::vec3 &position) {
            auto textEntity = basicEntityRepo.createEntity(entities::TextSprite, layerID);
            basicEntityRepo.setEntityText(textEntity, fontManager.createTextFromString(text, position));

            return textEntity;
        }
    }

}