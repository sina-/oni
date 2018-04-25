#include <oni-core/entities/create-entity.h>
#include <oni-core/entities/basic-entity-repo.h>
#include <oni-core/entities/vehicle-entity-repo.h>
#include <oni-core/graphics/font-manager.h>
// TODO: It sucks that I have to include shader.h just because scene-manager forward declares it and includes it in
// the scene-manager.cpp only.
#include <oni-core/graphics/shader.h>
#include <oni-core/graphics/batch-renderer-2d.h>
#include <oni-core/utils/oni-assert.h>
#include <oni-core/graphics/scene-manager.h>
#include <oni-core/physics/translation.h>

namespace oni {
    namespace entities {

        common::entityID createSpriteEntity(BasicEntityRepo &basicEntityRepo, components::ShaderID shaderID,
                                            const math::vec4 &color,
                                            const math::vec3 &position, const math::vec2 &size) {
            auto entity = basicEntityRepo.createEntity(entities::Sprite, shaderID);

            auto entityPlacement = components::Placement::fromPositionAndSize(position, size, 0.0f);
            components::Appearance entityAppearance{color};

            basicEntityRepo.setEntityPlacementLocal(entity, entityPlacement);
            basicEntityRepo.setEntityAppearance(entity, entityAppearance);

            return entity;
        }

        common::entityID createSpriteStaticEntity(BasicEntityRepo &basicEntityRepo,
                                                  graphics::SceneManager &sceneManager,
                                                  const math::vec4 &color,
                                                  const math::vec2 &size,
                                                  const math::vec3 &positionInWorld) {
            auto shaderID = sceneManager.requestShaderID(components::VertexType::COLOR_VERTEX);
            auto entity = basicEntityRepo.createEntity(entities::SpriteStatic, shaderID);
            auto entityAppearance = components::Appearance {color};

            auto entityPlacementLocal = components::Placement::fromSize(size, 0.0f);
            auto entityPlacementWorld = entityPlacementLocal;

            physics::Translation::localToWorld(positionInWorld, entityPlacementWorld);

            basicEntityRepo.setEntityPlacementLocal(entity, entityPlacementLocal);
            basicEntityRepo.setEntityPlacementWorld(entity, entityPlacementWorld);
            basicEntityRepo.setEntityAppearance(entity, entityAppearance);

            return entity;
        }

        common::entityID createTexturedEntity(BasicEntityRepo &basicEntityRepo, components::ShaderID shaderID,
                                              const components::Texture &entityTexture, const math::vec3 &position,
                                              const math::vec2 &size) {
            auto entity = basicEntityRepo.createEntity(entities::TexturedSprite, shaderID);

            auto entityPlacement = components::Placement::fromPositionAndSize(position, size, 0.0f);

            basicEntityRepo.setEntityPlacementLocal(entity, entityPlacement);
            basicEntityRepo.setEntityTexture(entity, entityTexture);

            return entity;
        }

        common::entityID createTexturedStaticEntity(BasicEntityRepo &basicEntityRepo,
                                                    graphics::SceneManager &sceneManager,
                                                    const components::Texture &entityTexture,
                                                    const math::vec2 &size,
                                                    const math::vec3 &positionInWorld) {
            auto shaderID = sceneManager.requestShaderID(components::VertexType::TEXTURE_VERTEX);
            auto entity = basicEntityRepo.createEntity(entities::TexturedSpriteStatic, shaderID);

            auto entityPlacementLocal = components::Placement::fromSize(size, 0.0f);
            auto entityPlacementWorld = entityPlacementLocal;

            physics::Translation::localToWorld(positionInWorld, entityPlacementWorld);

            basicEntityRepo.setEntityPlacementLocal(entity, entityPlacementLocal);
            basicEntityRepo.setEntityPlacementWorld(entity, entityPlacementWorld);
            basicEntityRepo.setEntityTexture(entity, entityTexture);

            return entity;
        }

        common::entityID createTextEntity(BasicEntityRepo &basicEntityRepo, graphics::FontManager &fontManager,
                                          components::ShaderID shaderID, const std::string &text,
                                          const math::vec3 &position) {
            auto textEntity = basicEntityRepo.createEntity(entities::TextSprite, shaderID);
            basicEntityRepo.setEntityText(textEntity, fontManager.createTextFromString(text, position));

            return textEntity;
        }

        common::entityID createTextStaticEntity(BasicEntityRepo &basicEntityRepo,
                                                graphics::SceneManager &sceneManager,
                                                graphics::FontManager &fontManager,
                                                const std::string &text,
                                                const math::vec3 &position,
                                                const math::vec2 &size,
                                                const math::vec3 &positionInWorld,
                                                std::string &&vertShader,
                                                std::string &&fragShader) {

            // TODO: Text does not have a local and world Placement, have to fix that before implementing
            // similar initialization and handling as normal static Textures.
            auto shaderID = sceneManager.requestShaderID(components::VertexType::TEXTURE_VERTEX);
            auto textEntity = basicEntityRepo.createEntity(entities::TextSprite, shaderID);
            basicEntityRepo.setEntityText(textEntity, fontManager.createTextFromString(text, position));

            return textEntity;

        }

        common::entityID createVehicleEntity(VehicleEntityRepo &vehicleEntityRepo, components::ShaderID shaderID,
                                             const components::Texture &entityTexture) {
            auto carConfig = components::CarConfig();
            auto entity = vehicleEntityRepo.createEntity(entities::DynamicTexturedSprite, shaderID);

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
            carConfig.maxSteer = 0.5f;
            carConfig.rollResist = 8.0f;

            // TODO: This is wrong. Car vertex position must be in local space starting from (0, 0).
            // Model matrix should move the car to this coordinates.
            auto carX = -carConfig.cgToRear;
            auto carY = -carConfig.halfWidth;
            auto carSizeX = carConfig.cgToRear + carConfig.cgToFront;
            auto carSizeY = carConfig.halfWidth * 2.0f;

            ONI_DEBUG_ASSERT(carSizeX - carConfig.cgToFront - carConfig.cgToRear < 0.00001f);

            auto entityPlacement = components::Placement::fromPositionAndSize(
                    math::vec3{static_cast<float> (carX), static_cast<float> (carY), 1.0f},
                    math::vec2{static_cast<float> (carSizeX), static_cast<float> (carSizeY)}, 0.0f);
            auto car = components::Car(carConfig);
            vehicleEntityRepo.setCar(entity, car);
            vehicleEntityRepo.setCarConfig(entity, carConfig);

            vehicleEntityRepo.setEntityPlacementLocal(entity, entityPlacement);
            vehicleEntityRepo.setEntityTexture(entity, entityTexture);

            return entity;
        }
    }

}