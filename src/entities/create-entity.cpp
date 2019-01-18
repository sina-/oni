#include <oni-core/entities/create-entity.h>

#include <Box2D/Box2D.h>

#include <oni-core/components/visual.h>
#include <oni-core/components/geometry.h>
#include <oni-core/physics/transformation.h>
#include <oni-core/entities/entity-manager.h>
#include <oni-core/components/snapshot-type.h>
#include <oni-core/components/hierarchy.h>

namespace oni {
    namespace entities {

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

        void destroyEntity(EntityManager &manager, common::EntityID entityID) {
            manager.destroy(entityID);
            manager.addDeletedEntity(entityID);
        }

        void assignShapeLocal(EntityManager &manager,
                              common::EntityID entityID,
                              const math::vec2 &size,
                              common::real32 z
                              ) {
            // NOTE: For dynamic entities, it is important to align object center to (0, 0) so that MVP transformation
            // works out without needing to translate the entity to the center before rotation and then back to its
            // position in the local space.
            auto halfSizeX = size.x / 2;
            auto halfSizeY = size.y / 2;
            auto shape = components::Shape::fromPositionAndSize(math::vec3{-halfSizeX, -halfSizeY, z}, size);
            manager.assign<components::Shape>(entityID, shape);
        }

        void assignShapeWorld(EntityManager &manager, common::EntityID entityID, const math::vec2 &size,
                              const math::vec3 &worldPos) {
            auto sizeWithZ = math::vec3{size.x, size.y, worldPos.z};
            auto shape = components::Shape::fromSizeAndRotation(sizeWithZ, 0);

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
            // TODO: Lot of hardcoded stuff here, these needs to be configurable.

            b2PolygonShape shape{};
            shape.SetAsBox(size.x / 2.0f, size.y / 2.0f);

            // NOTE: This is non-owning pointer. physicsWorld owns it.
            b2Body *body{};

            b2BodyDef bodyDef;
            bodyDef.bullet = highPrecisionPhysics;
            // TODO: This is just really a hack to distinguish between player car and the truck. Should have possibility
            // to provide is as part of physics definition.
            if (!highPrecisionPhysics) {
                bodyDef.linearDamping = 2.0f;
                bodyDef.angularDamping = 2.0f;
            }

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

                    b2FixtureDef collisionSensorDef;
                    collisionSensorDef.isSensor = true;
                    collisionSensorDef.shape = &shape;
                    collisionSensorDef.density = 1.0f;
                    collisionSensorDef.friction = 0.1;

                    body->CreateFixture(&fixtureDef);
                    body->CreateFixture(&collisionSensorDef);
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

            // TODO: Is there a better way to share the body pointer? Ownership is fucked up right now. Maybe
            // There is a way to request it from b2World?
            manager.assign<components::PhysicalProperties>(entityID, body);
        }

        void assignCar(EntityManager &manager, common::EntityID entityID, const math::vec3 &worldPos,
                       const components::CarConfig &carConfig) {
            auto car = components::Car(carConfig);
            car.position.x = worldPos.x;
            car.position.y = worldPos.y;
            manager.assign<components::Car>(entityID, car);
            manager.assign<components::CarConfig>(entityID, carConfig);
        }

        void removeShape(EntityManager &manager, common::EntityID entityID) {
            manager.remove<components::Shape>(entityID);
        }

        void removePlacement(EntityManager &manager, common::EntityID entityID) {
            manager.remove<components::Placement>(entityID);
        }

        void removeAppearance(EntityManager &manager, common::EntityID entityID) {
            manager.remove<components::Appearance>(entityID);
        }

        void removePhysicalProperties(EntityManager &manager, b2World &physicsWorld, common::EntityID entityID) {
            auto entityPhysicalProps = manager.get<components::PhysicalProperties>(entityID);
            physicsWorld.DestroyBody(entityPhysicalProps.body);
            manager.remove<components::PhysicalProperties>(entityID);
        }

        void removeTexture(EntityManager &manager, common::EntityID entityID) {
            manager.remove<components::Texture>(entityID);
        }

        void removeText(EntityManager &manager, common::EntityID entityID) {
            manager.remove<components::Text>(entityID);
        }

        void removeCar(EntityManager &manager, common::EntityID entityID) {
            manager.remove<components::Car>(entityID);
            manager.remove<components::CarConfig>(entityID);
        }

        void assignTransformationHierarchy(EntityManager &manager, common::EntityID parent, common::EntityID child) {
            if (manager.has<components::TransformChildren>(parent)) {
                auto transformChildren = manager.get<components::TransformChildren>(parent);
                transformChildren.children.emplace_back(child);
                manager.replace<components::TransformChildren>(parent, transformChildren);
            } else {
                auto transformChildren = components::TransformChildren{};
                transformChildren.children.emplace_back(child);
                manager.assign<components::TransformChildren>(parent, transformChildren);
            }

            assert(!manager.has<components::TransformParent>(child));

            auto transformParent = components::TransformParent{parent, math::mat4::identity()};
            manager.assign<components::TransformParent>(child, transformParent);
        }

        void removeTransformationHierarchy(EntityManager &manager, common::EntityID parent, common::EntityID child) {
            if (manager.has<components::TransformChildren>(parent)) {
                manager.remove<components::TransformChildren>(parent);
            }
            manager.remove<components::TransformParent>(child);
        }
    }


}