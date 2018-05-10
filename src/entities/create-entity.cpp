#include <oni-core/entities/create-entity.h>
#include <oni-core/graphics/font-manager.h>

// TODO: It sucks that I have to include shader.h just because scene-manager forward declares it and includes it in
// the scene-manager.cpp only.
#include <oni-core/graphics/shader.h>
#include <oni-core/graphics/batch-renderer-2d.h>
#include <oni-core/utils/oni-assert.h>
#include <oni-core/graphics/scene-manager.h>
#include <oni-core/physics/transformation.h>

namespace oni {
    namespace entities {

        entityID createSpriteEntity(entt::DefaultRegistry &registry,
                                    const math::vec4 &color,
                                    const math::vec2 &size,
                                    const math::vec3 &positionInWorld) {
            auto entity = registry.create();

            auto entityShapeWorld = components::Shape::fromSizeAndRotation(size, 0);
            auto entityAppearance = components::Appearance{color};
            auto entityColorShader = components::TagColorShaded{};

            physics::Transformation::localToWorldTranslation(positionInWorld, entityShapeWorld);

            registry.assign<components::Shape>(entity, entityShapeWorld);
            registry.assign<components::Appearance>(entity, entityAppearance);
            registry.assign<components::TagColorShaded>(entity, entityColorShader);

            return entity;
        }

        entityID createSpriteStaticEntity(entt::DefaultRegistry &registry,
                                          const math::vec4 &color,
                                          const math::vec2 &size,
                                          const math::vec3 &positionInWorld) {
            auto entity = registry.create();

            auto entityAppearance = components::Appearance {color};
            auto entityShapeWorld = components::Shape::fromSizeAndRotation(size, 0);
            auto entityStatic = components::TagStatic{};
            auto entityColorShader = components::TagColorShaded{};

            physics::Transformation::localToWorldTranslation(positionInWorld, entityShapeWorld);

            registry.assign<components::Shape>(entity, entityShapeWorld);
            registry.assign<components::Appearance>(entity, entityAppearance);
            registry.assign<components::TagColorShaded>(entity, entityColorShader);
            // TODO: Is this the correct usage of tags?
            registry.assign<components::TagStatic>(entity, entityStatic);

            return entity;
        }

        // TODO: The use of heading is totally bunkers. Sometimes its in radians and other times in degree!
        entityID createTexturedDynamicEntity(entt::DefaultRegistry &registry, const components::Texture &entityTexture,
                                             const math::vec2 &size, const math::vec3 &positionInWorld,
                                             const common::real32 heading,
                                             const math::vec3 &scale) {
            auto entity = registry.create();

            auto entityShape = components::Shape::fromSizeAndRotation(size, heading);
            auto entityDynamic = components::TagDynamic{};
            auto entityTextureShaded = components::TagTextureShaded{};

            auto entityPlacement = components::Placement{positionInWorld, heading, scale};

            registry.assign<components::Shape>(entity, entityShape);
            registry.assign<components::Texture>(entity, entityTexture);
            registry.assign<components::Placement>(entity, entityPlacement);
            registry.assign<components::TagTextureShaded>(entity, entityTextureShaded);
            registry.assign<components::TagDynamic>(entity, entityDynamic);

            return entity;
        }

        entityID createTexturedStaticEntity(entt::DefaultRegistry &registry,
                                            const components::Texture &entityTexture,
                                            const math::vec2 &size,
                                            const math::vec3 &positionInWorld) {
            auto entity = registry.create();
            auto entityShapeWorld = components::Shape::fromSizeAndRotation(size, 0);
            auto entityStatic = components::TagStatic{};
            auto entityTextureShaded = components::TagTextureShaded{};

            physics::Transformation::localToWorldTranslation(positionInWorld, entityShapeWorld);

            registry.assign<components::Shape>(entity, entityShapeWorld);
            registry.assign<components::Texture>(entity, entityTexture);
            registry.assign<components::TagTextureShaded>(entity, entityTextureShaded);
            registry.assign<components::TagStatic>(entity, entityStatic);

            return entity;
        }

        entityID createTextEntity(entt::DefaultRegistry &registry, graphics::FontManager &fontManager,
                                  const std::string &text,
                                  const math::vec3 &position) {
            // TODO: This is incompelte
            auto entity = registry.create();

            return entity;
        }

        entityID createTextStaticEntity(entt::DefaultRegistry &registry,
                                        graphics::FontManager &fontManager,
                                        const std::string &text,
                                        const math::vec3 &position,
                                        const math::vec2 &size,
                                        const math::vec3 &positionInWorld) {

            auto entity = registry.create();
            // TODO: Text does not have a local and world Shape, have to fix that before implementing
            // similar initialization and handling as normal static Textures.

            return entity;

        }

        entityID createVehicleEntity(entt::DefaultRegistry &registry,
                                     const components::Texture &entityTexture) {
            auto entity = registry.create();

            auto carConfig = components::CarConfig();
            auto entityVehicleTag = components::TagVehicle{};
            auto entityDynamicTag = components::TagDynamic{};
            auto entityTextureShaded = components::TagTextureShaded{};

            // TODO: this should be defined by the user of this function
            carConfig.cgToRear = 1.25f;
            carConfig.cgToFront = 1.25f;
            carConfig.cgToFrontAxle = 1.15f;
            carConfig.cgToRearAxle = 1.00f;
            carConfig.halfWidth = 0.55f;
            carConfig.inertialScale = 0.5f;
            carConfig.lockGrip = 0.2f;
            carConfig.tireGrip = 3.0f;
            carConfig.engineForce = 10000;
            carConfig.brakeForce = 4000;
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

            auto entityShapeWorld = components::Shape::fromPositionAndSize(
                    math::vec3{static_cast<common::real32> (carX), static_cast<common::real32> (carY)},
                    math::vec2{static_cast<common::real32> (carSizeX), static_cast<common::real32> (carSizeY)});
            auto car = components::Car(carConfig);

            auto entityPlacement = components::Placement{math::vec3{0.0f, 0.0f, 1.0f}, 0.0f, {1.0f, 1.0f, 0.0f}};

            registry.assign<components::Shape>(entity, entityShapeWorld);
            registry.assign<components::Placement>(entity, entityPlacement);
            registry.assign<components::Texture>(entity, entityTexture);
            registry.assign<components::TagTextureShaded>(entity, entityTextureShaded);
            registry.assign<components::Car>(entity, car);
            registry.assign<components::CarConfig>(entity, carConfig);
            registry.assign<components::TagVehicle>(entity, entityVehicleTag);
            registry.assign<components::TagDynamic>(entity, entityDynamicTag);

            return entity;
        }
    }

}