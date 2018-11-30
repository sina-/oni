#include <oni-core/entities/create-entity.h>

#include <Box2D/Box2D.h>

#include <oni-core/components/visual.h>
#include <oni-core/components/geometry.h>
#include <oni-core/physics/transformation.h>
#include <oni-core/graphics/font-manager.h>
#include <oni-core/entities/entity-manager.h>
#include <oni-core/components/world-data-status.h>

namespace oni {
    namespace entities {


        void assignText(EntityManager &manager,
                        graphics::FontManager &fontManager,
                        common::EntityID entity,
                        const std::string &text,
                        const math::vec3 &worldPos) {
            auto textComponent = fontManager.createTextFromString(text, worldPos);
            manager.assign<components::Text>(entity, textComponent);
        }

        common::EntityID createVehicleEntity(EntityManager &manager,
                                             b2World &physicsWorld,
                                             const components::CarConfig &carConfig) {
            auto carPosition = math::vec3{-70.f, -30.f, 1.f};

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
            car.position.x = carPosition.x;
            car.position.y = carPosition.y;

            auto entityPlacement = components::Placement{carPosition};

            b2BodyDef bodyDef;

            bodyDef.position.x = carPosition.x;
            bodyDef.position.y = carPosition.y;
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
            carCollisionSensorDef.friction = 0.1;

            b2FixtureDef carFixtureDef;
            carFixtureDef.shape = &carShape;
            carFixtureDef.density = 1.0f;
            carFixtureDef.friction = 0.1;

            body->CreateFixture(&carFixtureDef);
            body->CreateFixture(&carCollisionSensorDef);

            // TODO: Is there a better way to share the body pointer? Ownership is fucked up right now. Maybe
            // There is a way to request it from b2World?
            // NOTE: This is non-owning pointer. physicsWorld owns it.
            auto entityPhysicalProps = components::PhysicalProperties{body};

            auto entity = manager.create();
            manager.assign<components::PhysicalProperties>(entity, entityPhysicalProps);
            manager.assign<components::Shape>(entity, entityShapeWorld);
            manager.assign<components::Placement>(entity, entityPlacement);
            manager.assign<components::Car>(entity, car);
            manager.assign<components::CarConfig>(entity, carConfig);
            manager.assign<components::Tag_Vehicle>(entity);
            manager.assign<components::Tag_Dynamic>(entity);
            manager.assign<components::Tag_NewEntity>(entity);

            return entity;
        }

        void deleteVehicleEntity(EntityManager &manager, b2World &physicsWorld, common::EntityID entityID) {
            auto lock = manager.scopedLock();

            auto entityPhysicalProps = manager.get<components::PhysicalProperties>(entityID);
            physicsWorld.DestroyBody(entityPhysicalProps.body);
            manager.remove<components::PhysicalProperties>(entityID);
            manager.remove<components::Shape>(entityID);
            manager.remove<components::Placement>(entityID);
            manager.remove<components::Car>(entityID);
            manager.remove<components::CarConfig>(entityID);
            manager.remove<components::Tag_Vehicle>(entityID);
            manager.remove<components::Tag_Dynamic>(entityID);
            manager.destroy(entityID);

            manager.addDeletedEntity(entityID);
        }

        void assignTextureToLoad(EntityManager &manager, common::EntityID entity, const std::string &path) {
            components::Texture texture;
            texture.filePath = path;
            texture.status = components::TextureStatus::NEEDS_LOADING_USING_PATH;
            manager.assign<components::Texture>(entity, texture);
            manager.assign<components::Tag_TextureShaded>(entity);
        }

        void assignTextureLoaded(EntityManager &manager, common::EntityID entity, const components::Texture &texture) {
            manager.assign<components::Texture>(entity, texture);
            manager.assign<components::Tag_TextureShaded>(entity);
        }

        common::EntityID createEntity(EntityManager &manager) {
            auto entityID = manager.create();
            manager.assign<components::Tag_NewEntity>(entityID);
            return entityID;
        }

        void assignShapeLocal(EntityManager &manager,
                              common::EntityID entityID,
                              const math::vec2 &size) {
            // NOTE: For dynamic entities, it is important to align object center to (0, 0) so that MVP transformation
            // works out without needing to translate the entity to the center before rotation and then back to its
            // position in the local space.
            auto halfSizeX = size.x / 2;
            auto halfSizeY = size.y / 2;
            auto shape = components::Shape::fromPositionAndSize(math::vec3{-halfSizeX, -halfSizeY, 1.0}, size);
            manager.assign<components::Shape>(entityID, shape);
        }

        void assignShapeWorld(EntityManager &manager, common::EntityID entityID, const math::vec2 &size,
                              const math::vec3 &worldPos) {
            auto shape = components::Shape::fromSizeAndRotation(size, 0);

            physics::Transformation::localToWorldTranslation(worldPos, shape);
            manager.assign<components::Shape>(entityID, shape);
        }

        void assignPlacement(EntityManager &manager,
                             common::EntityID entityID,
                             const math::vec3 &worldPos,
                             const math::vec3 &scale,
                             common::real32 heading) {
            manager.assign<components::Placement>(entityID, worldPos, heading, scale);
        }

        void assignAppearance(EntityManager &manager, common::EntityID entityID, const math::vec4 &color) {
            manager.assign<components::Appearance>(entityID, color);
            manager.assign<components::Tag_ColorShaded>(entityID);
        }

        void assignPhysicalProperties(EntityManager &manager,
                                      b2World &physicsWorld,
                                      common::EntityID entityID,
                                      const math::vec3 &worldPos,
                                      const math::vec2 &size,
                                      components::BodyType bodyType,
                                      bool highPrecisionPhysics) {
            b2PolygonShape shape{};
            shape.SetAsBox(size.x / 2.0f, size.y / 2.0f);
            b2Body *body{};

            b2BodyDef bodyDef;
            bodyDef.bullet = highPrecisionPhysics;
            // TODO: Lot of hardcoded stuff here, these needs to be configurable.
            bodyDef.linearDamping = 2.0f;
            bodyDef.angularDamping = 2.0f;

            b2FixtureDef fixtureDef;
            // NOTE: Box2D will create a copy of the shape, so it is safe to pass a local ref.
            fixtureDef.shape = &shape;
            fixtureDef.density = 1.0f;
            fixtureDef.friction = 0.1;

            switch (bodyType) {
                case components::BodyType::DYNAMIC : {
                    bodyDef.position.x = worldPos.x;
                    bodyDef.position.y = worldPos.y;
                    bodyDef.type = b2_dynamicBody;
                    body = physicsWorld.CreateBody(&bodyDef);
                    body->CreateFixture(&fixtureDef);
                    break;
                }
                case components::BodyType::STATIC: {
                    // NOTE: for static entities position in world is the bottom left corner of the sprite. But
                    // bodyDef.position is the center of gravity of the entity.
                    bodyDef.position.x = worldPos.x + size.x / 2.0f;
                    bodyDef.position.y = worldPos.y + size.y / 2.0f;
                    bodyDef.type = b2_staticBody;
                    body = physicsWorld.CreateBody(&bodyDef);
                    body->CreateFixture(&shape, 0.f);
                    break;
                }
                case components::BodyType::KINEMATIC: {
                    bodyDef.position.x = worldPos.x;
                    bodyDef.position.y = worldPos.y;
                    bodyDef.type = b2_kinematicBody;
                    body = physicsWorld.CreateBody(&bodyDef);
                    body->CreateFixture(&fixtureDef);
                    break;
                }
            }
            manager.assign<components::PhysicalProperties>(entityID, body);
        }
    }


}