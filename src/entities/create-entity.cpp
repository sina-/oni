#include <oni-core/entities/create-entity.h>

#include <Box2D/Box2D.h>

#include <oni-core/components/visual.h>
#include <oni-core/components/geometry.h>
#include <oni-core/physics/transformation.h>
#include <oni-core/entities/entity-manager.h>
#include <oni-core/components/world-data-status.h>

namespace oni {
    namespace entities {

        common::EntityID createSpriteEntity(EntityManager &manager,
                                            const math::vec4 &color,
                                            const math::vec2 &size,
                                            const math::vec3 &positionInWorld) {

            auto entityShapeWorld = components::Shape::fromSizeAndRotation(size, 0);
            auto entityAppearance = components::Appearance{color};

            physics::Transformation::localToWorldTranslation(positionInWorld, entityShapeWorld);

            auto lock = manager.scopedLock();
            auto entity = manager.create();
            manager.assign<components::Shape>(entity, entityShapeWorld);
            manager.assign<components::Appearance>(entity, entityAppearance);
            manager.assign<components::TagColorShaded>(entity);

            return entity;
        }

        common::EntityID createSpriteStaticEntity(EntityManager &manager,
                                                  const math::vec4 &color,
                                                  const math::vec2 &size,
                                                  const math::vec3 &positionInWorld) {

            auto entityAppearance = components::Appearance{color};
            auto entityShapeWorld = components::Shape::fromSizeAndRotation(size, 0);

            physics::Transformation::localToWorldTranslation(positionInWorld, entityShapeWorld);

            auto lock = manager.scopedLock();
            auto entity = manager.create();
            manager.assign<components::Shape>(entity, entityShapeWorld);
            manager.assign<components::Appearance>(entity, entityAppearance);
            manager.assign<components::TagColorShaded>(entity);
            // TODO: Is this the correct usage of tags?
            manager.assign<components::TagStatic>(entity);
            manager.assign<components::TagAddNewEntities>(entity);

            return entity;
        }

        // TODO: The use of heading is totally bunkers. Sometimes its in radians and other times in degree!
        common::EntityID createDynamicEntity(EntityManager &manager, const math::vec2 &size,
                                             const math::vec3 &positionInWorld,
                                             const common::real32 heading, const math::vec3 &scale) {

            // NOTE: For dynamic entities, it is important to align object center to (0, 0) so that MVP transformation
            // works out without needing to translate the entity to the center before rotation and then back to its
            // position in the local space.
            auto halfSizeX = size.x / 2;
            auto halfSizeY = size.y / 2;
            auto entityShape = components::Shape::fromPositionAndSize(math::vec3{-halfSizeX, -halfSizeY, 1.0}, size);

            auto entityPlacement = components::Placement{positionInWorld, heading, scale};

            auto lock = manager.scopedLock();
            auto entity = manager.create();
            manager.assign<components::Shape>(entity, entityShape);
            manager.assign<components::Placement>(entity, entityPlacement);
            manager.assign<components::TagTextureShaded>(entity);
            manager.assign<components::TagDynamic>(entity);
            manager.assign<components::TagAddNewEntities>(entity);

            return entity;
        }

        // TODO: The use of heading is totally bunkers. Sometimes its in radians and other times in degree!
        common::EntityID createDynamicPhysicsEntity(EntityManager &manager, b2World &physicsWorld,
                                                    const math::vec2 &size,
                                                    const math::vec3 &positionInWorld, const common::real32 heading,
                                                    const math::vec3 &scale) {

            // NOTE: For dynamic entities, it is important to align object center to (0, 0) so that MVP transformation
            // works out without needing to translate the entity to the center before rotation and then back to its
            // position in the local space.
            auto halfSizeX = size.x / 2;
            auto halfSizeY = size.y / 2;
            auto entityShape = components::Shape::fromPositionAndSize(math::vec3{-halfSizeX, -halfSizeY, 1.0}, size);

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

            auto lock = manager.scopedLock();
            auto entity = manager.create();
            manager.assign<components::PhysicalProperties>(entity, entityPhysicalProps);
            manager.assign<components::Shape>(entity, entityShape);
            manager.assign<components::Placement>(entity, entityPlacement);
            manager.assign<components::TagTextureShaded>(entity);
            manager.assign<components::TagDynamic>(entity);
            manager.assign<components::TagAddNewEntities>(entity);

            return entity;
        }

        common::EntityID createStaticEntity(EntityManager &manager, const math::vec2 &size,
                                            const math::vec3 &positionInWorld) {
            auto entityShapeWorld = components::Shape::fromSizeAndRotation(size, 0);

            physics::Transformation::localToWorldTranslation(positionInWorld, entityShapeWorld);

            auto lock = manager.scopedLock();
            auto entity = manager.create();
            manager.assign<components::Shape>(entity, entityShapeWorld);
            manager.assign<components::TagTextureShaded>(entity);
            manager.assign<components::TagStatic>(entity);
            manager.assign<components::TagAddNewEntities>(entity);

            return entity;
        }

        common::EntityID createStaticPhysicsEntity(EntityManager &manager, b2World &physicsWorld,
                                                   const math::vec2 &size,
                                                   const math::vec3 &positionInWorld) {
            auto entityShapeWorld = components::Shape::fromSizeAndRotation(size, 0);
            physics::Transformation::localToWorldTranslation(positionInWorld, entityShapeWorld);

            b2BodyDef bodyDef;
            // NOTE: for static entities position in world is the bottom left corner of the sprite. But
            // bodyDef.position is the center of gravity of the entity.
            bodyDef.position.x = positionInWorld.x + size.x / 2.0f;
            bodyDef.position.y = positionInWorld.y + size.y / 2.0f;
            bodyDef.type = b2_staticBody;
            auto *body = physicsWorld.CreateBody(&bodyDef);

            auto boxShape = b2PolygonShape();
            boxShape.SetAsBox(size.x / 2.0f, size.y / 2.0f);
            body->CreateFixture(&boxShape, 0.0f);

            // TODO: Is there a better way to share the body pointer? Ownership is fucked up right now. Maybe
            // There is a way to request it from b2World?
            // NOTE: This is non-owning pointer. physicsWorld owns it.
            auto entityPhysicalProps = components::PhysicalProperties{body};

            auto lock = manager.scopedLock();
            auto entity = manager.create();
            manager.assign<components::PhysicalProperties>(entity, entityPhysicalProps);
            manager.assign<components::Shape>(entity, entityShapeWorld);
            manager.assign<components::TagTextureShaded>(entity);
            manager.assign<components::TagStatic>(entity);
            manager.assign<components::TagAddNewEntities>(entity);

            return entity;
        }

        common::EntityID createTextStaticEntity(EntityManager &manager,
                                                graphics::FontManager &fontManager,
                                                const std::string &text,
                                                const math::vec3 &position,
                                                const math::vec2 &size,
                                                const math::vec3 &positionInWorld) {

            auto lock = manager.scopedLock();
            auto entity = manager.create();
            // TODO: Text does not have a local and world Shape, have to fix that before implementing
            // similar initialization and handling as normal static Textures.

            return entity;

        }

        common::EntityID
        createVehicleEntity(EntityManager &manager, b2World &physicsWorld, const components::CarConfig &carConfig) {

            // TODO: This is wrong. Car vertex position must be in local space starting from (0, 0).
            // Model matrix should move the car to this coordinates.
            auto carX = -carConfig.cgToRear;
            auto carY = -carConfig.halfWidth;
            auto carSizeX = carConfig.cgToRear + carConfig.cgToFront;
            auto carSizeY = carConfig.halfWidth * 2.0f;

            assert(carSizeX - carConfig.cgToFront - carConfig.cgToRear < 0.00001f);

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

            auto carShape = b2PolygonShape();
            auto carSize = entityShapeWorld.getSize();
            carShape.SetAsBox(carSize.x / 2.0f, carSize.y / 2.0f);

            b2FixtureDef carCollisionSensorDef;
            carCollisionSensorDef.isSensor = true;
            carCollisionSensorDef.shape = &carShape;
            carCollisionSensorDef.density = 1.0f;
            carCollisionSensorDef.friction = 0.3;

            b2FixtureDef carFixtureDef;
            carFixtureDef.shape = &carShape;
            carFixtureDef.density = 1.0f;
            carFixtureDef.friction = 0.3;

            body->CreateFixture(&carFixtureDef);
            body->CreateFixture(&carCollisionSensorDef);

            // TODO: Is there a better way to share the body pointer? Ownership is fucked up right now. Maybe
            // There is a way to request it from b2World?
            // NOTE: This is non-owning pointer. physicsWorld owns it.
            auto entityPhysicalProps = components::PhysicalProperties{body};

            auto lock = manager.scopedLock();
            auto entity = manager.create();
            manager.assign<components::PhysicalProperties>(entity, entityPhysicalProps);
            manager.assign<components::Shape>(entity, entityShapeWorld);
            manager.assign<components::Placement>(entity, entityPlacement);
            manager.assign<components::TagTextureShaded>(entity);
            manager.assign<components::Car>(entity, car);
            manager.assign<components::CarConfig>(entity, carConfig);
            manager.assign<components::TagVehicle>(entity);
            manager.assign<components::TagDynamic>(entity);
            manager.assign<components::TagAddNewEntities>(entity);

            return entity;
        }

        void deleteVehicleEntity(EntityManager &manager, b2World &physicsWorld, common::EntityID entityID) {
            auto lock = manager.scopedLock();

            auto entityPhysicalProps = manager.get<components::PhysicalProperties>(entityID);
            physicsWorld.DestroyBody(entityPhysicalProps.body);
            manager.remove<components::PhysicalProperties>(entityID);
            manager.remove<components::Shape>(entityID);
            manager.remove<components::Placement>(entityID);
            manager.remove<components::TagTextureShaded>(entityID);
            manager.remove<components::Car>(entityID);
            manager.remove<components::CarConfig>(entityID);
            manager.remove<components::TagVehicle>(entityID);
            manager.remove<components::TagDynamic>(entityID);
            manager.destroy(entityID);

            manager.addDeletedEntity(entityID);
        }

        void assignTexture(EntityManager &manager, common::EntityID entity, const components::Texture &texture) {
            auto lock = manager.scopedLock();
            manager.assign<components::Texture>(entity, texture);
        }

    }


}