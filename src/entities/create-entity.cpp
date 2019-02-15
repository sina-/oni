#include <oni-core/entities/create-entity.h>

#include <Box2D/Box2D.h>

#include <oni-core/component/visual.h>
#include <oni-core/component/geometry.h>
#include <oni-core/physics/transformation.h>
#include <oni-core/entities/entity-manager.h>
#include <oni-core/component/snapshot-type.h>
#include <oni-core/component/hierarchy.h>

namespace oni {
    namespace entities {

        void assignTextureToLoad(EntityManager &manager, common::EntityID entity, const std::string &path) {
            component::Texture texture;
            texture.filePath = path;
            texture.status = component::TextureStatus::NEEDS_LOADING_USING_PATH;
            manager.assign<component::Texture>(entity, texture);
            manager.assign<component::Tag_TextureShaded>(entity);
        }

        void assignTextureLoaded(EntityManager &manager, common::EntityID entity, const component::Texture &texture) {
            manager.assign<component::Texture>(entity, texture);
            manager.assign<component::Tag_TextureShaded>(entity);
        }

        common::EntityID createEntity(EntityManager &manager, bool tagAsNew) {
            auto entityID = manager.create();
            if (tagAsNew) {
                manager.assign<component::Tag_NewEntity>(entityID);
            }
            return entityID;
        }

        void destroyEntity(EntityManager &manager, common::EntityID entityID) {
            if (!manager.has<component::Tag_NewEntity>(entityID)) {
                manager.addDeletedEntity(entityID);
            }
            manager.destroy(entityID);
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
            auto sprite = component::Shape::fromPositionAndSize(math::vec3{-halfSizeX, -halfSizeY, z}, size);
            manager.assign<component::Shape>(entityID, sprite);
        }

        void assignShapeRotatedLocal(EntityManager &manager, common::EntityID entityID, const math::vec2 &size,
                                     common::real32 z, common::real32 angle) {
            auto halfSizeX = size.x / 2;
            auto halfSizeY = size.y / 2;
            auto sprite = component::Shape::fromPositionAndSize(math::vec3{-halfSizeX, -halfSizeY, z}, size);
            physics::Transformation::shapeTransformation(
                    math::mat4::rotation(math::toRadians(angle), math::vec3{0.f, 0.f, 1.f}), sprite);
            manager.assign<component::Shape>(entityID, sprite);
        }

        void assignShapeWorld(EntityManager &manager, common::EntityID entityID, const math::vec2 &size,
                              const math::vec3 &worldPos) {
            auto sizeWithZ = math::vec3{size.x, size.y, worldPos.z};
            auto sprite = component::Shape::fromSizeAndRotation(sizeWithZ, 0);

            physics::Transformation::localToWorldTranslation(worldPos, sprite);
            manager.assign<component::Shape>(entityID, sprite);
        }

        void assignPoint(EntityManager &manager,
                         common::EntityID entityID,
                         const math::vec3 &pos) {
            component::Point point{pos.x, pos.y, pos.z};
            manager.assign<component::Point>(entityID, point);
        }

        void assignPlacement(EntityManager &manager,
                             common::EntityID entityID,
                             const math::vec3 &worldPos,
                             const math::vec3 &scale,
                             common::real32 heading) {
            manager.assign<component::Placement>(entityID, worldPos, heading, scale);
        }

        void assignAppearance(EntityManager &manager, common::EntityID entityID, const math::vec4 &color) {
            manager.assign<component::Appearance>(entityID, color);
            manager.assign<component::Tag_ColorShaded>(entityID);
        }

        void assignPhysicalProperties(EntityManager &manager,
                                      b2World &physicsWorld,
                                      common::EntityID entityID,
                                      const math::vec3 &worldPos,
                                      const math::vec2 &size,
                                      common::real32 heading,
                                      component::PhysicalProperties &properties
        ) {
            b2PolygonShape shape{};
            shape.SetAsBox(size.x / 2.0f, size.y / 2.0f);

            // NOTE: This is non-owning pointer. physicsWorld owns it.
            b2Body *body{};

            b2BodyDef bodyDef;
            bodyDef.bullet = properties.highPrecision;
            bodyDef.angle = heading;
            bodyDef.linearDamping = properties.linearDamping;
            bodyDef.angularDamping = properties.angularDamping;

            b2FixtureDef fixtureDef;
            // NOTE: Box2D will create a copy of the shape, so it is safe to pass a local ref.
            fixtureDef.shape = &shape;
            fixtureDef.density = properties.density;
            fixtureDef.friction = properties.friction;

            switch (properties.bodyType) {
                case component::BodyType::DYNAMIC : {
                    bodyDef.position.x = worldPos.x;
                    bodyDef.position.y = worldPos.y;
                    bodyDef.type = b2_dynamicBody;
                    body = physicsWorld.CreateBody(&bodyDef);

                    b2FixtureDef collisionSensor;
                    collisionSensor.isSensor = true;
                    collisionSensor.shape = &shape;
                    collisionSensor.density = properties.density;
                    collisionSensor.friction = properties.friction;

                    if (!properties.collisionWithinCategory) {
                        collisionSensor.filter.groupIndex = -static_cast<common::int16>(properties.physicalCategory);
                    }

                    body->CreateFixture(&fixtureDef);
                    body->CreateFixture(&collisionSensor);
                    break;
                }
                case component::BodyType::STATIC: {
                    // NOTE: for static entities position in world is the bottom left corner of the sprite. But
                    // bodyDef.position is the center of gravity of the entity.
                    bodyDef.position.x = worldPos.x + size.x / 2.0f;
                    bodyDef.position.y = worldPos.y + size.y / 2.0f;
                    bodyDef.type = b2_staticBody;
                    body = physicsWorld.CreateBody(&bodyDef);
                    body->CreateFixture(&shape, 0.f);
                    break;
                }
                case component::BodyType::KINEMATIC: {
                    bodyDef.position.x = worldPos.x;
                    bodyDef.position.y = worldPos.y;
                    bodyDef.type = b2_kinematicBody;
                    body = physicsWorld.CreateBody(&bodyDef);
                    body->CreateFixture(&fixtureDef);
                    break;
                }
            }
            properties.body = body;

            // TODO: Is there a better way to share the body pointer? Ownership is fucked up right now. Maybe
            // There is a way to request it from b2World?
            manager.assign<component::PhysicalProperties>(entityID, properties);

            // NOTE: Need to re-read it since assign creates a copy
            auto &props = manager.get<component::PhysicalProperties>(entityID);
            body->SetUserData(static_cast<void *>(&props.colliding));
        }

        void assignCar(EntityManager &manager, common::EntityID entityID, const math::vec3 &worldPos,
                       const component::CarConfig &carConfig) {
            auto car = component::Car(carConfig);
            car.position.x = worldPos.x;
            car.position.y = worldPos.y;
            manager.assign<component::Car>(entityID, car);
            manager.assign<component::CarConfig>(entityID, carConfig);
        }

        void assignParticle(EntityManager &manager, common::EntityID entityID, const math::vec3 &worldPos,
                            common::real32 life) {
            component::Particle particle{};
            particle.pos = worldPos;
            particle.life = life;
            manager.assign<component::Particle>(entityID, particle);
            manager.assign<component::Tag_Particle>(entityID);
        }

        void removeShape(EntityManager &manager, common::EntityID entityID) {
            manager.remove<component::Shape>(entityID);
        }

        void removePlacement(EntityManager &manager, common::EntityID entityID) {
            manager.remove<component::Placement>(entityID);
        }

        void removeAppearance(EntityManager &manager, common::EntityID entityID) {
            manager.remove<component::Appearance>(entityID);
        }

        void removePhysicalProperties(EntityManager &manager, b2World &physicsWorld, common::EntityID entityID) {
            auto entityPhysicalProps = manager.get<component::PhysicalProperties>(entityID);
            physicsWorld.DestroyBody(entityPhysicalProps.body);
            manager.remove<component::PhysicalProperties>(entityID);
        }

        void removeTexture(EntityManager &manager, common::EntityID entityID) {
            manager.remove<component::Texture>(entityID);
        }

        void removeText(EntityManager &manager, common::EntityID entityID) {
            manager.remove<component::Text>(entityID);
        }

        void removeCar(EntityManager &manager, common::EntityID entityID) {
            manager.remove<component::Car>(entityID);
            manager.remove<component::CarConfig>(entityID);
        }

        void assignTransformationHierarchy(EntityManager &manager, common::EntityID parent, common::EntityID child) {
            if (manager.has<component::TransformChildren>(parent)) {
                auto transformChildren = manager.get<component::TransformChildren>(parent);
                transformChildren.children.emplace_back(child);
                manager.replace<component::TransformChildren>(parent, transformChildren);
            } else {
                auto transformChildren = component::TransformChildren{};
                transformChildren.children.emplace_back(child);
                manager.assign<component::TransformChildren>(parent, transformChildren);
            }

            assert(!manager.has<component::TransformParent>(child));

            auto transformParent = component::TransformParent{parent, math::mat4::identity()};
            manager.assign<component::TransformParent>(child, transformParent);
        }

        void removeTransformationHierarchy(EntityManager &manager, common::EntityID parent, common::EntityID child) {
            if (manager.has<component::TransformChildren>(parent)) {
                manager.remove<component::TransformChildren>(parent);
            }
            manager.remove<component::TransformParent>(child);
        }
    }
}