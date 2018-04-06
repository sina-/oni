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
                                                        const components::Texture &entityTexture, const math::vec3 &position,
                                                        const float scalingFactor) {
            auto entity = basicEntityRepo.createEntity(entities::TexturedSprite, layerID);

            auto size = math::vec2(entityTexture.width * scalingFactor, entityTexture.height * scalingFactor);

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
                                               const components::Texture &entityTexture, const float scalingFactor) {
            auto carConfig = components::CarConfig();
            auto entity = vehicleEntityRepo.createEntity(entities::DynamicTexturedSprite, layerID);

            carConfig.cgToRear = 1.0f;
            carConfig.cgToFront = 1.0f;
            carConfig.cgToFrontAxle = 0.75f;
            carConfig.cgToRearAxle = 0.75f;
            carConfig.inertialScale = 0.5f;
            carConfig.lockGrip = 0.2f;
            carConfig.tireGrip = 4.0f;
            carConfig.engineForce = 8000;
            carConfig.brakeForce = 4000;
            // NOTE: These numbers should match carConfig.cgToFront and cgToRear and all the other dimension data.
            // For example if cgToFront is 2.0m then texture width should be 2 * 20 * 2 = 80px
            // and if carConfig.halfWidth is 0.9m then texture height should be 0.9 * 20 * 2 = 18px
            auto carX = -entityTexture.width * scalingFactor / 2;
            auto carY = -entityTexture.height * scalingFactor / 2;
            auto carSizeX = entityTexture.width * scalingFactor;
            auto carSizeY = entityTexture.height * scalingFactor;

            ONI_DEBUG_ASSERT(carSizeX - carConfig.cgToFront - carConfig.cgToRear < 0.00001f);
            auto entityPlacement = components::Placement(math::vec3(carX, carY, 1.0f),
                                                         math::vec2(carSizeX, carSizeY));
            auto car = components::Car(carConfig);
            vehicleEntityRepo.setCar(entity, car);
            vehicleEntityRepo.setCarConfig(entity, carConfig);

            vehicleEntityRepo.setEntityPlacement(entity, entityPlacement);
            vehicleEntityRepo.setEntityTexture(entity, entityTexture);

            return entity;
        }
    }

}