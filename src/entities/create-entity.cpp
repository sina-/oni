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

        entities::entityID createTextEntity(BasicEntityRepo &basicEntityRepo, graphics::FontManager &fontManager,
                                            components::LayerID layerID, const std::string &text,
                                            const math::vec3 &position) {
            auto textEntity = basicEntityRepo.createEntity(entities::TextSprite, layerID);
            basicEntityRepo.setEntityText(textEntity, fontManager.createTextFromString(text, position));

            return textEntity;
        }

        entities::entityID createVehicleEntity(VehicleEntityRepo &vehicleEntityRepo, components::LayerID layerID,
                                               const components::Texture &entityTexture,
                                               const components::CarConfig &carConfig) {
            auto entity = vehicleEntityRepo.createEntity(entities::DynamicTexturedSprite, layerID);


            auto carX = -carConfig.cgToRear * carConfig.scaleMultiplierX;
            auto carY = -carConfig.halfWidth * carConfig.scaleMultiplierY;
            auto carSizeX = (carConfig.cgToRear + carConfig.cgToFront) * carConfig.scaleMultiplierX;
            auto carSizeY = carConfig.halfWidth * 2 * carConfig.scaleMultiplierY;

            auto entityPlacement = components::Placement(math::vec3((float) carX, (float) carY, 1.0f),
                                                         math::vec2((float) carSizeX, (float) carSizeY));
            auto car = components::Car(carConfig);
            vehicleEntityRepo.setCar(entity, car);
            vehicleEntityRepo.setCarConfig(entity, carConfig);

            vehicleEntityRepo.setEntityPlacement(entity, entityPlacement);
            vehicleEntityRepo.setEntityTexture(entity, entityTexture);

            return entity;
        }
    }

}