#include <entt/entt.hpp>

#include <oni-core/entities/create-entity.h>
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

        entityID createSpriteEntity(entt::DefaultRegistry &registry,
                                    graphics::SceneManager &sceneManager,
                                    const math::vec4 &color,
                                    const math::vec2 &size,
                                    const math::vec3 &positionInWorld) {
            auto entity = registry.create();

            auto shaderID = sceneManager.requestShaderID(components::VertexType::COLOR_VERTEX);
            auto entityPlacementWorld = components::Placement::fromSize(size, 0.0f);
            auto entityAppearance = components::Appearance{color};

            physics::Translation::localToWorld(positionInWorld, entityPlacementWorld);

            registry.assign<components::Placement>(entity, entityPlacementWorld);
            registry.assign<components::Appearance>(entity, entityAppearance);
            registry.assign<components::Renderer>(entity, shaderID);

            return entity;
        }

        entityID createSpriteStaticEntity(entt::DefaultRegistry &registry,
                                          graphics::SceneManager &sceneManager,
                                          const math::vec4 &color,
                                          const math::vec2 &size,
                                          const math::vec3 &positionInWorld) {
            auto entity = registry.create();

            auto shaderID = sceneManager.requestShaderID(components::VertexType::COLOR_VERTEX);
            auto entityAppearance = components::Appearance {color};
            auto entityPlacementWorld = components::Placement::fromSize(size, 0.0f);
            auto entityStatic = components::Static{};

            physics::Translation::localToWorld(positionInWorld, entityPlacementWorld);

            registry.assign<components::Placement>(entity, entityPlacementWorld);
            registry.assign<components::Appearance>(entity, entityAppearance);
            registry.assign<components::Renderer>(entity, shaderID);
            // TODO: Is this the correct usage of tags?
            registry.assign<components::Static>(entity, entityStatic);

            return entity;
        }

        entityID createTexturedDynamicEntity(entt::DefaultRegistry &registry,
                                             graphics::SceneManager &sceneManager,
                                             const components::Texture &entityTexture,
                                             const math::vec2 &size,
                                             const math::vec3 &positionInWorld) {
            auto entity = registry.create();

            auto shaderID = sceneManager.requestShaderID(components::VertexType::TEXTURE_VERTEX);
            auto entityPlacementWorld = components::Placement::fromSize(size, 0.0f);
            auto entityDynamic = components::Dynamic{};

            physics::Translation::localToWorld(positionInWorld, entityPlacementWorld);

            registry.assign<components::Placement>(entity, entityPlacementWorld);
            registry.assign<components::Texture>(entity, entityTexture);
            registry.assign<components::Renderer>(entity, shaderID);
            registry.assign<components::Dynamic>(entity, entityDynamic);

            return entity;
        }

        entityID createTexturedStaticEntity(entt::DefaultRegistry &registry,
                                            graphics::SceneManager &sceneManager,
                                            const components::Texture &entityTexture,
                                            const math::vec2 &size,
                                            const math::vec3 &positionInWorld) {
            auto entity = registry.create();
            auto shaderID = sceneManager.requestShaderID(components::VertexType::TEXTURE_VERTEX);
            auto entityPlacementWorld = components::Placement::fromSize(size, 0.0f);
            auto entityStatic = components::Static{};

            physics::Translation::localToWorld(positionInWorld, entityPlacementWorld);

            registry.assign<components::Placement>(entity, entityPlacementWorld);
            registry.assign<components::Texture>(entity, entityTexture);
            registry.assign<components::Renderer>(entity, shaderID);
            registry.assign<components::Static>(entity, entityStatic);

            return entity;
        }

        entityID createTextEntity(entt::DefaultRegistry &registry, graphics::FontManager &fontManager,
                                  components::ShaderID shaderID, const std::string &text,
                                  const math::vec3 &position) {
            // TODO: This is incompelte
            auto entity = registry.create();

            return entity;
        }

        entityID createTextStaticEntity(entt::DefaultRegistry &registry,
                                        graphics::SceneManager &sceneManager,
                                        graphics::FontManager &fontManager,
                                        const std::string &text,
                                        const math::vec3 &position,
                                        const math::vec2 &size,
                                        const math::vec3 &positionInWorld,
                                        std::string &&vertShader,
                                        std::string &&fragShader) {

            auto entity = registry.create();
            // TODO: Text does not have a local and world Placement, have to fix that before implementing
            // similar initialization and handling as normal static Textures.
            auto shaderID = sceneManager.requestShaderID(components::VertexType::TEXTURE_VERTEX);

            return entity;

        }

        entityID createVehicleEntity(entt::DefaultRegistry &registry, graphics::SceneManager &sceneManager,
                                     const components::Texture &entityTexture) {
            auto entity = registry.create();

            auto carConfig = components::CarConfig();
            auto shaderID = sceneManager.requestShaderID(components::VertexType::TEXTURE_VERTEX);
            auto entityDynamic = components::Dynamic{};

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

            auto entityPlacementWorld = components::Placement::fromPositionAndSize(
                    math::vec3{static_cast<float> (carX), static_cast<float> (carY), 1.0f},
                    math::vec2{static_cast<float> (carSizeX), static_cast<float> (carSizeY)}, 0.0f);
            auto car = components::Car(carConfig);

            registry.assign<components::Placement>(entity, entityPlacementWorld);
            registry.assign<components::Texture>(entity, entityTexture);
            registry.assign<components::Renderer>(entity, shaderID);
            registry.assign<components::Car>(entity, car);
            registry.assign<components::CarConfig>(entity, carConfig);
            registry.assign<components::Dynamic>(entity, entityDynamic);

            return entity;
        }
    }

}