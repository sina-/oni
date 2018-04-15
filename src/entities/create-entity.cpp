#include <oni-core/entities/create-entity.h>
#include <oni-core/entities/basic-entity-repo.h>
#include <oni-core/entities/vehicle-entity-repo.h>
#include <oni-core/graphics/font-manager.h>
#include <oni-core/utils/oni-assert.h>

namespace oni {
    namespace entities {

        entities::entityID createSpriteEntity(BasicEntityRepo &basicEntityRepo, components::LayerID layerID,
                                              const math::vec4 &color,
                                              const math::vec3 &position, const math::vec2 &size) {
            auto entity = basicEntityRepo.createEntity(entities::Sprite, layerID);

            auto entityPlacement = components::Placement::fromPositionAndSize(position, size);
            components::Appearance entityAppearance{color};

            basicEntityRepo.setEntityPlacement(entity, entityPlacement);
            basicEntityRepo.setEntityAppearance(entity, entityAppearance);

            return entity;
        }

        entities::entityID createTexturedEntity(BasicEntityRepo &basicEntityRepo, components::LayerID layerID,
                                                const components::Texture &entityTexture, const math::vec3 &position,
                                                const math::vec2 &size) {
            auto entity = basicEntityRepo.createEntity(entities::TexturedSprite, layerID);

            auto entityPlacement = components::Placement::fromPositionAndSize(position, size);

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
                                               const components::Texture &entityTexture) {
            auto carConfig = components::CarConfig();
            auto entity = vehicleEntityRepo.createEntity(entities::DynamicTexturedSprite, layerID);

            // TODO: this should be defined by the user of this function
            carConfig.cgToRear = 1.25f;
            carConfig.cgToFront = 1.25f;
            carConfig.cgToFrontAxle = 1.15f;
            carConfig.cgToRearAxle = 1.15f;
            carConfig.halfWidth = 0.55f;
            carConfig.inertialScale = 0.5f;
            carConfig.lockGrip = 0.2f;
            carConfig.tireGrip = 3.0f;
            carConfig.engineForce = 10000;
            carConfig.brakeForce = 1000;
            carConfig.cornerStiffnessRear = 5.5f;
            carConfig.cornerStiffnessFront = 5.0f;
            carConfig.maxSteer = 0.6f;
            carConfig.rollResist = 8.0f;

            auto carX = -carConfig.cgToRear;
            auto carY = -carConfig.halfWidth;
            auto carSizeX = carConfig.cgToRear + carConfig.cgToFront;
            auto carSizeY = carConfig.halfWidth * 2.0f;

            ONI_DEBUG_ASSERT(carSizeX - carConfig.cgToFront - carConfig.cgToRear < 0.00001f);

            auto entityPlacement = components::Placement::fromPositionAndSize(
                    math::vec3{(float) (carX), (float) (carY), 1.0f},
                    math::vec2{(float) (carSizeX), (float) (carSizeY)});
            auto car = components::Car(carConfig);
            vehicleEntityRepo.setCar(entity, car);
            vehicleEntityRepo.setCarConfig(entity, carConfig);

            vehicleEntityRepo.setEntityPlacement(entity, entityPlacement);
            vehicleEntityRepo.setEntityTexture(entity, entityTexture);

            return entity;
        }
    }

}