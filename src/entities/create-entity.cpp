#include <oni-core/entities/create-entity.h>

namespace oni {
    namespace entities {

        entities::entityID createSpriteEntity(World &world, components::LayerID layerID, const math::vec4 &color,
                                              const math::vec3 &position, const math::vec2 &size) {
            auto entity = world.createEntity(entities::Sprite, layerID);

            auto entityPlacement = components::Placement();
            entityPlacement.vertexA = math::vec3(position.x, position.y, position.z);
            entityPlacement.vertexB = math::vec3(position.x, position.y + size.y, position.z);
            entityPlacement.vertexC = math::vec3(position.x + size.x, position.y + size.y, position.z);
            entityPlacement.vertexD = math::vec3(position.x + size.x, position.y, position.z);

            auto entityAppearance = components::Appearance();
            entityAppearance.color = color;

            world.setEntityPlacement(entity, entityPlacement);
            world.setEntityAppearance(entity, entityAppearance);

            return entity;
        }

        entities::entityID createTexturedEntity(World &world, components::LayerID layerID,
                                                const components::Texture &entityTexture,
                                                const math::vec3 &position, const math::vec2 &size) {
            auto entity = world.createEntity(entities::TexturedSprite, layerID);

            auto entityPlacement = components::Placement();
            entityPlacement.vertexA = math::vec3(position.x, position.y, position.z);
            entityPlacement.vertexB = math::vec3(position.x, position.y + size.y, position.z);
            entityPlacement.vertexC = math::vec3(position.x + size.x, position.y + size.y, position.z);
            entityPlacement.vertexD = math::vec3(position.x + size.x, position.y, position.z);

            world.setEntityPlacement(entity, entityPlacement);
            world.setEntityTexture(entity, entityTexture);

            return entity;
        }

        entities::entityID createDynamicTexturedEntity(World &world, components::LayerID layerID,
                                                       const components::Texture &entityTexture,
                                                       const math::vec3 &position, const math::vec2 &size) {
            auto entity = world.createEntity(entities::DynamicTexturedSprite, layerID);

            auto entityPlacement = components::Placement();
            entityPlacement.vertexA = math::vec3(position.x, position.y, position.z);
            entityPlacement.vertexB = math::vec3(position.x, position.y + size.y, position.z);
            entityPlacement.vertexC = math::vec3(position.x + size.x, position.y + size.y, position.z);
            entityPlacement.vertexD = math::vec3(position.x + size.x, position.y, position.z);

            auto entityVelocity = components::Velocity();
            entityVelocity.magnitude = 8.0f;
            entityVelocity.direction = math::vec3(0.0f, 0.0f, 0.0f);

            world.setEntityPlacement(entity, entityPlacement);
            world.setEntityVelocity(entity, entityVelocity);
            world.setEntityTexture(entity, entityTexture);

            return entity;
        }

        entities::entityID createTextEntity(World &world, graphics::FontManager &fontManager,
                                            components::LayerID layerID, const std::string &text,
                                            const math::vec3 &position) {
            auto textEntity = world.createEntity(entities::TextSprite, layerID);
            world.setEntityText(textEntity, fontManager.createTextFromString(text, position));

            return textEntity;
        }
    }

}