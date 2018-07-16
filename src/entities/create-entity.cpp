#include <oni-core/entities/create-entity.h>

#include <Box2D/Box2D.h>

// TODO: It sucks that I have to include shader.h just because scene-manager forward declares it and includes it in
// the scene-manager.cpp only.
#include <oni-core/graphics/shader.h>
#include <oni-core/graphics/batch-renderer-2d.h>
#include <oni-core/graphics/font-manager.h>
#include <oni-core/utils/oni-assert.h>
#include <oni-core/graphics/scene-manager.h>
#include <oni-core/physics/transformation.h>
#include <oni-core/graphics/texture.h>

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

            auto entityAppearance = components::Appearance{color};
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
        entityID createDynamicEntity(entt::DefaultRegistry &registry, const math::vec2 &size,
                                     const math::vec3 &positionInWorld,
                                     const common::real32 heading, const math::vec3 &scale) {
            auto entity = registry.create();

            // NOTE: For dynamic entities, it is important to align object center to (0, 0) so that MVP transformation
            // works out without needing to translate the entity to the center before rotation and then back to its
            // position in the local space.
            auto halfSizeX = size.x / 2;
            auto halfSizeY = size.y / 2;
            auto entityShape = components::Shape::fromPositionAndSize(math::vec3{-halfSizeX, -halfSizeY, 1.0}, size);
            auto entityDynamic = components::TagDynamic{};
            auto entityTextureShaded = components::TagTextureShaded{};

            auto entityPlacement = components::Placement{positionInWorld, heading, scale};

            registry.assign<components::Shape>(entity, entityShape);
            registry.assign<components::Placement>(entity, entityPlacement);
            registry.assign<components::TagTextureShaded>(entity, entityTextureShaded);
            registry.assign<components::TagDynamic>(entity, entityDynamic);

            return entity;
        }

        // TODO: The use of heading is totally bunkers. Sometimes its in radians and other times in degree!
        entityID createDynamicPhysicsEntity(entt::DefaultRegistry &registry, b2World &physicsWorld,
                                            const math::vec2 &size,
                                            const math::vec3 &positionInWorld, const common::real32 heading,
                                            const math::vec3 &scale) {
            auto entity = registry.create();

            // NOTE: For dynamic entities, it is important to align object center to (0, 0) so that MVP transformation
            // works out without needing to translate the entity to the center before rotation and then back to its
            // position in the local space.
            auto halfSizeX = size.x / 2;
            auto halfSizeY = size.y / 2;
            auto entityShape = components::Shape::fromPositionAndSize(math::vec3{-halfSizeX, -halfSizeY, 1.0}, size);
            auto entityDynamic = components::TagDynamic{};
            auto entityTextureShaded = components::TagTextureShaded{};

            auto entityPlacement = components::Placement{positionInWorld, heading, scale};

            b2BodyDef bodyDef;
            bodyDef.position.x = positionInWorld.x;
            bodyDef.position.y = positionInWorld.y;
            bodyDef.linearDamping = 2.0f;
            bodyDef.angularDamping = 2.0f;
            bodyDef.type = b2_dynamicBody;

            auto *body = physicsWorld.CreateBody(&bodyDef);

            auto truckShape = std::make_unique<b2PolygonShape>();
            truckShape->SetAsBox(size.x / 2.0f, size.y / 2.0f);

            b2FixtureDef truckFixtureDef;
            truckFixtureDef.shape = truckShape.get();
            truckFixtureDef.density = 10.0f;
            truckFixtureDef.friction = 10.9f;
            body->CreateFixture(&truckFixtureDef);

            // TODO: Is there a better way to share the body pointer? Ownership is fucked up right now. Maybe
            // There is a way to request it from b2World?
            // NOTE: This is non-owning pointer. physicsWorld owns it.
            auto entityPhysicalProps = components::PhysicalProperties{body};

            registry.assign<components::PhysicalProperties>(entity, entityPhysicalProps);
            registry.assign<components::Shape>(entity, entityShape);
            registry.assign<components::Placement>(entity, entityPlacement);
            registry.assign<components::TagTextureShaded>(entity, entityTextureShaded);
            registry.assign<components::TagDynamic>(entity, entityDynamic);

            return entity;
        }

        entityID createStaticEntity(entt::DefaultRegistry &registry, const math::vec2 &size,
                                    const math::vec3 &positionInWorld) {
            auto entity = registry.create();
            auto entityShapeWorld = components::Shape::fromSizeAndRotation(size, 0);
            auto entityStatic = components::TagStatic{};
            auto entityTextureShaded = components::TagTextureShaded{};

            physics::Transformation::localToWorldTranslation(positionInWorld, entityShapeWorld);

            registry.assign<components::Shape>(entity, entityShapeWorld);
            registry.assign<components::TagTextureShaded>(entity, entityTextureShaded);
            registry.assign<components::TagStatic>(entity, entityStatic);

            return entity;
        }

        entityID createStaticPhysicsEntity(entt::DefaultRegistry &registry, b2World &physicsWorld,
                                           const math::vec2 &size,
                                           const math::vec3 &positionInWorld) {
            auto entity = registry.create();
            auto entityShapeWorld = components::Shape::fromSizeAndRotation(size, 0);
            auto entityStatic = components::TagStatic{};
            auto entityTextureShaded = components::TagTextureShaded{};

            physics::Transformation::localToWorldTranslation(positionInWorld, entityShapeWorld);

            b2BodyDef bodyDef;
            // NOTE: for static entities position in world is the bottom left corner of the sprite. But
            // bodyDef.position is the center of gravity of the entity.
            bodyDef.position.x = positionInWorld.x + size.x / 2.0f;
            bodyDef.position.y = positionInWorld.y + size.y / 2.0f;
            bodyDef.type = b2_staticBody;
            auto *body = physicsWorld.CreateBody(&bodyDef);

            auto boxShape = std::make_unique<b2PolygonShape>();
            boxShape->SetAsBox(size.x / 2.0f, size.y / 2.0f);
            body->CreateFixture(boxShape.get(), 0.0f);

            // TODO: Is there a better way to share the body pointer? Ownership is fucked up right now. Maybe
            // There is a way to request it from b2World?
            // NOTE: This is non-owning pointer. physicsWorld owns it.
            auto entityPhysicalProps = components::PhysicalProperties{body};

            registry.assign<components::PhysicalProperties>(entity, entityPhysicalProps);
            registry.assign<components::Shape>(entity, entityShapeWorld);
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

        entityID createVehicleEntity(entt::DefaultRegistry &registry, b2World &physicsWorld) {
            auto entity = registry.create();

            auto carConfig = components::CarConfig();
            auto entityVehicleTag = components::TagVehicle{};
            auto entityTextureShaded = components::TagTextureShaded{};
            auto entityDynamic = components::TagDynamic{};

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
            carConfig.wheelRadius = 0.25f;

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

            b2BodyDef bodyDef;

            bodyDef.position.x = 0.0f;
            bodyDef.position.y = 0.0f;
            bodyDef.type = b2_dynamicBody;
            bodyDef.bullet = true;
            auto *body = physicsWorld.CreateBody(&bodyDef);

            auto carShape = std::make_unique<b2PolygonShape>();
            auto carSize = entityShapeWorld.getSize();
            carShape->SetAsBox(carSize.x / 2.0f, carSize.y / 2.0f);

            b2FixtureDef carCollisionSensorDef;
            carCollisionSensorDef.isSensor = true;
            carCollisionSensorDef.shape = carShape.get();
            carCollisionSensorDef.density = 1.0f;
            carCollisionSensorDef.friction = 0.3;

            b2FixtureDef carFixtureDef;
            carFixtureDef.shape = carShape.get();
            carFixtureDef.density = 1.0f;
            carFixtureDef.friction = 0.3;

            body->CreateFixture(&carFixtureDef);
            body->CreateFixture(&carCollisionSensorDef);

            // TODO: Is there a better way to share the body pointer? Ownership is fucked up right now. Maybe
            // There is a way to request it from b2World?
            // NOTE: This is non-owning pointer. physicsWorld owns it.
            auto entityPhysicalProps = components::PhysicalProperties{body};


            registry.assign<components::PhysicalProperties>(entity, entityPhysicalProps);
            registry.assign<components::Shape>(entity, entityShapeWorld);
            registry.assign<components::Placement>(entity, entityPlacement);
            registry.assign<components::TagTextureShaded>(entity, entityTextureShaded);
            registry.assign<components::Car>(entity, car);
            registry.assign<components::CarConfig>(entity, carConfig);
            registry.assign<components::TagVehicle>(entity, entityVehicleTag);
            registry.assign<components::TagDynamic>(entity, entityDynamic);

            return entity;
        }

        void assignTexture(entt::DefaultRegistry &registry, entityID entity, const components::Texture &texture) {
            registry.assign<components::Texture>(entity, texture);
        }
    }

}