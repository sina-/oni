#include <oni-core/entities/entity-factory.h>

#include <Box2D/Box2D.h>

#include <oni-core/component/geometry.h>
#include <oni-core/component/hierarchy.h>
#include <oni-core/component/visual.h>
#include <oni-core/math/rand.h>
#include <oni-core/component/gameplay.h>

namespace oni {
    namespace entities {
        EntityFactory::EntityFactory(const math::ZLayerManager &zLayerManager,
                                     b2World &physicsWorld) :
                mZLayerManager(zLayerManager),
                mPhysicsWorld{physicsWorld} {
            mRand = std::make_unique<math::Rand>(0);
            mManager = std::make_unique<oni::entities::EntityManager>();
        }

        EntityManager &EntityFactory::getEntityManager() {
            return *mManager;
        }

        common::EntityID EntityFactory::createEntity() {
            auto entityID = mManager->create();
            mManager->assign<component::Tag_NewEntity>(entityID);
            return entityID;
        }

        void EntityFactory::destroyEntity(common::EntityID entityID) {
            // TODO: Can we remove this check?
            if (!mManager->has<component::Tag_NewEntity>(entityID)) {
                mManager->addDeletedEntity(entityID);
            }
            mManager->destroy(entityID);
        }

        void EntityFactory::_removeEntity(common::EntityID entityID, component::EntityType entityType) {
            switch (entityType) {
                case component::EntityType::RACE_CAR: {
                    _removeEntity<component::EntityType::RACE_CAR>(entityID);
                    break;
                }
                case component::EntityType::WALL: {
                    _removeEntity<component::EntityType::WALL>(entityID);
                    break;
                }
                case component::EntityType::VEHICLE_GUN: {
                    _removeEntity<component::EntityType::VEHICLE_GUN>(entityID);
                    break;
                }
                case component::EntityType::VEHICLE_TIRE_FRONT: {
                    _removeEntity<component::EntityType::VEHICLE_TIRE_FRONT>(entityID);
                    break;
                }
                case component::EntityType::VEHICLE_TIRE_REAR: {
                    _removeEntity<component::EntityType::VEHICLE_TIRE_REAR>(entityID);
                    break;
                }
                case component::EntityType::SIMPLE_BULLET: {
                    _removeEntity<component::EntityType::SIMPLE_BULLET>(entityID);
                    break;
                }
                case component::EntityType::ROAD:
                case component::EntityType::VEHICLE:
                case component::EntityType::BACKGROUND:
                case component::EntityType::SKID_LINE:
                case component::EntityType::UI:
                case component::EntityType::SIMPLE_SPRITE:
                case component::EntityType::SIMPLE_PARTICLE:
                case component::EntityType::TEXT:
                case component::EntityType::WORLD_CHUNK:
                case component::EntityType::UNKNOWN: {
                    assert(false);
                    break;
                }
            }
            removeComponent<component::EntityType>(entityID);
            destroyEntity(entityID);
        }

        template<>
        void EntityFactory::_createEntity<component::EntityType::RACE_CAR>(common::EntityID entityID,
                                                                           const math::vec3 &pos,
                                                                           const math::vec2 &size,
                                                                           const common::real32 &heading,
                                                                           const std::string &textureID) {
            auto &carConfig = createComponent<component::CarConfig>(entityID);
            carConfig.cgToRear = size.x / 2;
            carConfig.cgToFront = size.x / 2;
            carConfig.halfWidth = size.y / 2;
            assert(size.x - carConfig.cgToFront - carConfig.cgToRear < 0.00001f);

            auto &placement = createComponent<component::Placement>(entityID);
            placement.position = pos;
            placement.rotation = heading;

            auto &properties = createComponent<component::PhysicalProperties>(entityID);
            properties.physicalCategory = oni::component::PhysicalCategory::RACE_CAR;
            properties.bodyType = component::BodyType::DYNAMIC;
            properties.highPrecision = true;

            // TODO: Does this make sense? Or should it be same as createComponent?
            auto *body = createPhysicalBody(pos, size, heading, properties);

            auto &texture = createComponent<component::Texture>(entityID);
            texture.filePath = textureID;
            texture.status = component::TextureStatus::NEEDS_LOADING_USING_PATH;

            auto &car = createComponent<component::Car>(entityID);
            car.position.x = pos.x;
            car.position.y = pos.y;
            car.applyConfiguration(carConfig);

            auto &shape = createComponent<component::Shape>(entityID);
            shape.setZ(pos.z);
            shape.setSizeFromOrigin(size);
            shape.centerAlign();

            auto carLap = createComponent<component::CarLapInfo>(entityID);
            carLap.entityID = entityID;
            carLap.lap = 0;
            carLap.bestLapTimeS = 0;
            carLap.lapTimeS = 0;

            createComponent<component::TransformChildren>(entityID);
            createComponent<component::EntityAttachment>(entityID);

            assignTag<component::Tag_TextureShaded>(entityID);
            assignTag<component::Tag_Dynamic>(entityID);
        }

        template<>
        void EntityFactory::_createEntity<component::EntityType::VEHICLE>(common::EntityID entityID,
                                                                          const math::vec3 &pos,
                                                                          const math::vec2 &size,
                                                                          const common::real32 &heading,
                                                                          const std::string &textureID) {
            auto &properties = createComponent<component::PhysicalProperties>(entityID);
            properties.friction = 1.f;
            properties.density = 0.1f;
            properties.angularDamping = 2.f;
            properties.linearDamping = 2.f;
            properties.highPrecision = false;
            properties.bodyType = component::BodyType::DYNAMIC;
            properties.physicalCategory = component::PhysicalCategory::VEHICLE;

            auto &placement = createComponent<component::Placement>(entityID);
            placement.position = pos;
            placement.rotation = heading;

            auto *body = createPhysicalBody(pos, size, heading, properties);

            auto &texture = createComponent<component::Texture>(entityID);
            texture.filePath = textureID;
            texture.status = component::TextureStatus::NEEDS_LOADING_USING_PATH;

            auto &shape = createComponent<component::Shape>(entityID);
            shape.setZ(pos.z);
            shape.setSizeFromOrigin(size);
            shape.centerAlign();

            assignTag<component::Tag_TextureShaded>(entityID);
            assignTag<component::Tag_Dynamic>(entityID);
        }

        template<>
        void EntityFactory::_createEntity<component::EntityType::VEHICLE_GUN>(common::EntityID entityID,
                                                                              const oni::math::vec3 &pos,
                                                                              const oni::math::vec2 &size,
                                                                              const common::real32 &heading,
                                                                              const std::string &textureID) {
            auto &placement = createComponent<component::Placement>(entityID);
            placement.position = pos;
            placement.rotation = heading;

            auto &texture = createComponent<component::Texture>(entityID);
            texture.filePath = textureID;
            texture.status = component::TextureStatus::NEEDS_LOADING_USING_PATH;

            auto &shape = createComponent<component::Shape>(entityID);
            shape.setZ(pos.z);
            shape.setSizeFromOrigin(size);
            shape.centerAlign();

            createComponent<component::EntityAttachee>(entityID);
            createComponent<component::TransformParent>(entityID);
            createComponent<component::GunCoolDown>(entityID);

            assignTag<component::Tag_Dynamic>(entityID);
            assignTag<component::Tag_TextureShaded>(entityID);
        }

        template<>
        void EntityFactory::_createEntity<component::EntityType::VEHICLE_TIRE_FRONT>(common::EntityID entityID,
                                                                                     const math::vec3 &pos,
                                                                                     const math::vec2 &size,
                                                                                     const common::real32 &heading,
                                                                                     const std::string &textureID) {
            auto &placement = createComponent<component::Placement>(entityID);
            placement.position = pos;
            placement.rotation = heading;

            auto &texture = createComponent<component::Texture>(entityID);
            texture.filePath = textureID;
            texture.status = component::TextureStatus::NEEDS_LOADING_USING_PATH;

            auto &shape = createComponent<component::Shape>(entityID);
            shape.setZ(pos.z);
            shape.setSizeFromOrigin(size);
            shape.centerAlign();

            createComponent<component::EntityAttachee>(entityID);
            createComponent<component::TransformParent>(entityID);

            assignTag<component::Tag_Dynamic>(entityID);
            assignTag<component::Tag_TextureShaded>(entityID);
        }

        template<>
        void EntityFactory::_createEntity<component::EntityType::VEHICLE_TIRE_REAR>(common::EntityID entityID,
                                                                                    const math::vec3 &pos,
                                                                                    const math::vec2 &size,
                                                                                    const common::real32 &heading,
                                                                                    const std::string &textureID) {
            _createEntity<component::EntityType::VEHICLE_TIRE_FRONT>(entityID, pos, size, heading, textureID);
        }

        template<>
        void EntityFactory::_createEntity<component::EntityType::WALL>(common::EntityID entityID,
                                                                       const math::vec3 &worldPos,
                                                                       const math::vec2 &size,
                                                                       const common::real32 &heading,
                                                                       const std::string &textureID) {
            auto &properties = createComponent<component::PhysicalProperties>(entityID);
            properties.highPrecision = false;
            properties.bodyType = oni::component::BodyType::STATIC;
            properties.physicalCategory = oni::component::PhysicalCategory::WALL;
            auto *body = createPhysicalBody(worldPos, size, heading, properties);

            auto &shape = createComponent<component::Shape>(entityID);
            shape.setZ(worldPos.z);
            shape.setSizeFromOrigin(size);
            shape.moveToWorldCoordinates(worldPos);

            auto &texture = createComponent<component::Texture>(entityID);
            texture.filePath = textureID;
            texture.status = component::TextureStatus::NEEDS_LOADING_USING_PATH;

            assignTag<component::Tag_Static>(entityID);
            assignTag<component::Tag_TextureShaded>(entityID);
        }

        template<>
        void EntityFactory::_createEntity<component::EntityType::SIMPLE_SPRITE>(common::EntityID entityID,
                                                                                const math::vec3 &worldPos,
                                                                                const math::vec2 &size,
                                                                                const common::real32 &heading,
                                                                                const math::vec4 &color) {
            auto &shape = createComponent<component::Shape>(entityID);
            shape.setZ(worldPos.z);
            shape.setSizeFromOrigin(size);
            shape.moveToWorldCoordinates(worldPos);

            auto &appearance = createComponent<component::Appearance>(entityID);
            appearance.color = color;

            assignTag<component::Tag_Static>(entityID);
            assignTag<component::Tag_ColorShaded>(entityID);
        }

        template<>
        void EntityFactory::_createEntity<component::EntityType::SIMPLE_SPRITE>(common::EntityID entityID,
                                                                                const math::vec3 &worldPos,
                                                                                const math::vec2 &size,
                                                                                const common::real32 &heading,
                                                                                const std::string &textureID) {
            auto &shape = createComponent<component::Shape>(entityID);
            shape.setZ(worldPos.z);
            shape.setSizeFromOrigin(size);
            shape.moveToWorldCoordinates(worldPos);

            auto &texture = createComponent<component::Texture>(entityID);
            texture.filePath = textureID;
            texture.status = component::TextureStatus::NEEDS_LOADING_USING_PATH;

            assignTag<component::Tag_Static>(entityID);
            assignTag<component::Tag_TextureShaded>(entityID);
        }

        template<>
        void EntityFactory::_createEntity<component::EntityType::SIMPLE_PARTICLE>(common::EntityID entityID,
                                                                                  const math::vec3 &worldPos,
                                                                                  const math::vec4 &color,
                                                                                  const bool &randomize) {
            auto &particle = createComponent<component::Particle>(entityID);
            particle.pos = worldPos;
            particle.age = 0.f;

            auto &appearance = createComponent<component::Appearance>(entityID);
            appearance.color = color;

            if (randomize) {
                particle.heading = mRand->nextReal32(0, 6.28f); // NOTE: 6.28f is 2 * pi in radians (360 degrees)
                particle.velocity = mRand->nextReal32(1.f, 7.f);
                particle.maxAge = mRand->nextReal32(0.2f, 1.f);
            }
            // NOTE: For now particles are sync'd with a game packet, don't need to serialize them through registry
            removeTag<component::Tag_NewEntity>(entityID);
        }

        template<>
        void EntityFactory::_createEntity<component::EntityType::SIMPLE_PARTICLE>(common::EntityID entityID,
                                                                                  const math::vec3 &worldPos,
                                                                                  const std::string &textureID,
                                                                                  const bool &randomize) {
            auto &particle = createComponent<component::Particle>(entityID);
            particle.pos = worldPos;
            particle.age = 0.f;

            auto &texture = createComponent<component::Texture>(entityID);
            texture.filePath = textureID;
            texture.status = component::TextureStatus::NEEDS_LOADING_USING_PATH;

            if (randomize) {
                particle.heading = mRand->nextReal32(0, 6.28f); // NOTE: 6.28f is 2 * pi in radians (360 degrees)
                particle.velocity = mRand->nextReal32(1.f, 7.f);
                particle.maxAge = mRand->nextReal32(0.2f, 1.f);
            }
            // NOTE: For now particles are sync'd with a game packet, don't need to serialize them through registry
            removeTag<component::Tag_NewEntity>(entityID);
        }

        template<>
        void EntityFactory::_createEntity<component::EntityType::SIMPLE_BULLET>(common::EntityID entityID,
                                                                                const math::vec3 &pos,
                                                                                const math::vec2 &size,
                                                                                const common::real32 &heading,
                                                                                const std::string &textureID,
                                                                                const common::real32 &velocity
        ) {
            auto &properties = createComponent<component::PhysicalProperties>(entityID);
            properties.friction = 1.f;
            properties.density = 0.1f;
            properties.angularDamping = 2.f;
            properties.linearDamping = 0.1f;
            properties.highPrecision = true;
            properties.bodyType = oni::component::BodyType::DYNAMIC;
            properties.physicalCategory = oni::component::PhysicalCategory::BULLET;
            properties.collisionWithinCategory = false;

            auto *body = createPhysicalBody(pos, size, heading, properties);

            body->ApplyForceToCenter(
                    b2Vec2(static_cast<common::real32>(cos(heading) * velocity),
                           static_cast<common::real32>(sin(heading) * velocity)),
                    true);

            auto &shape = createComponent<component::Shape>(entityID);
            shape.setZ(pos.z);
            shape.setSizeFromOrigin(size);
            shape.centerAlign();

            auto &placement = createComponent<component::Placement>(entityID);
            placement.position = pos;
            placement.rotation = heading;

            auto &trail = createComponent<component::Trail>(entityID);
            trail.previousPos.push_back(pos);
            trail.velocity.push_back(velocity);

            auto &texture = createComponent<component::Texture>(entityID);
            texture.filePath = textureID;
            texture.status = component::TextureStatus::NEEDS_LOADING_USING_PATH;

            assignTag<component::Tag_Dynamic>(entityID);
            assignTag<component::Tag_TextureShaded>(entityID);
        }

        template<>
        void EntityFactory::_createEntity<component::EntityType::TEXT>(common::EntityID entityID, const math::vec3 &pos,
                                                                       const std::string &text) {

            auto &textComponent = createComponent<component::Text>(entityID);
            textComponent.position = pos;
            textComponent.textContent = text;

            assignTag<component::Tag_Static>(entityID);
        }

        template<>
        void EntityFactory::_createEntity<component::EntityType::WORLD_CHUNK>(common::EntityID entityID,
                                                                              const math::vec3 &worldPos,
                                                                              const math::vec2 &size,
                                                                              const common::real32 &heading,
                                                                              const std::string &textureID) {
            auto &shape = createComponent<component::Shape>(entityID);
            shape.setZ(worldPos.z);
            shape.setSizeFromOrigin(size);
            shape.moveToWorldCoordinates(worldPos);

            auto &texture = createComponent<component::Texture>(entityID);
            texture.filePath = textureID;
            texture.status = component::TextureStatus::NEEDS_LOADING_USING_PATH;

            createComponent<component::Chunk>(entityID);

            assignTag<component::Tag_Static>(entityID);
            assignTag<component::Tag_TextureShaded>(entityID);
        }

        template<>
        void EntityFactory::_createEntity<component::EntityType::WORLD_CHUNK>(common::EntityID entityID,
                                                                              const math::vec3 &worldPos,
                                                                              const math::vec2 &size,
                                                                              const common::real32 &heading,
                                                                              const math::vec4 &color) {
            auto &shape = createComponent<component::Shape>(entityID);
            shape.setZ(worldPos.z);
            shape.setSizeFromOrigin(size);
            shape.moveToWorldCoordinates(worldPos);

            auto &appearance = createComponent<component::Appearance>(entityID);
            appearance.color = color;

            createComponent<component::Chunk>(entityID);

            assignTag<component::Tag_Static>(entityID);
            assignTag<component::Tag_ColorShaded>(entityID);
        }


        b2Body *EntityFactory::createPhysicalBody(const math::vec3 &worldPos,
                                                  const math::vec2 &size, common::real32 heading,
                                                  component::PhysicalProperties &properties) {
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
                    body = mPhysicsWorld.CreateBody(&bodyDef);

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
                    body = mPhysicsWorld.CreateBody(&bodyDef);
                    body->CreateFixture(&shape, 0.f);
                    break;
                }
                case component::BodyType::KINEMATIC: {
                    bodyDef.position.x = worldPos.x;
                    bodyDef.position.y = worldPos.y;
                    bodyDef.type = b2_kinematicBody;
                    body = mPhysicsWorld.CreateBody(&bodyDef);
                    body->CreateFixture(&fixtureDef);
                    break;
                }
                case component::BodyType::UNKNOWN: {
                    assert(false);
                    break;
                }
                default: {
                    assert(false);
                    break;
                }
            }
            properties.body = body;
            return body;
        }

        template<>
        void EntityFactory::_removeEntity<component::EntityType::RACE_CAR>(common::EntityID entityID) {
            auto &attachments = mManager->get<component::EntityAttachment>(entityID);
            for (common::size i = 0; i < attachments.entities.size(); ++i) {
                auto attachmentType = attachments.entityTypes[i];
                auto attachmentEntityID = attachments.entities[i];
                _removeEntity(attachmentEntityID, attachmentType);
            }
            removeComponent<component::EntityAttachment>(entityID);
            removeComponent<component::Shape>(entityID);
            removeComponent<component::Placement>(entityID);
            // TODO: When notifying clients of this, the texture in memory should be evicted.
            removeComponent<component::Texture>(entityID);
            removeComponent<component::TransformChildren>(entityID);
            removeComponent<component::Car>(entityID);
            removeComponent<component::CarConfig>(entityID);
            // TODO: Does it make sense to remove this? A player might leave but lap info should still be visible
            // Maybe I should not keep it as part of RACE_CAR entity?
            removeComponent<component::CarLapInfo>(entityID);

            removePhysicalBody(entityID);
            removeComponent<component::PhysicalProperties>(entityID);

            removeTag<component::Tag_Dynamic>(entityID);
            removeTag<component::Tag_TextureShaded>(entityID);
        }

        template<>
        void EntityFactory::_removeEntity<component::EntityType::VEHICLE_GUN>(common::EntityID entityID) {
            removeComponent<component::Placement>(entityID);
            removeComponent<component::Texture>(entityID);
            removeComponent<component::Shape>(entityID);
            removeComponent<component::EntityAttachee>(entityID);
            removeComponent<component::TransformParent>(entityID);
            removeComponent<component::GunCoolDown>(entityID);

            removeTag<component::Tag_Dynamic>(entityID);
            removeTag<component::Tag_TextureShaded>(entityID);
        }

        template<>
        void EntityFactory::_removeEntity<component::EntityType::VEHICLE_TIRE_FRONT>(common::EntityID entityID) {
            removeComponent<component::Placement>(entityID);
            removeComponent<component::Texture>(entityID);
            removeComponent<component::Shape>(entityID);
            removeComponent<component::EntityAttachee>(entityID);
            removeComponent<component::TransformParent>(entityID);

            removeTag<component::Tag_Dynamic>(entityID);
            removeTag<component::Tag_TextureShaded>(entityID);
        }

        template<>
        void EntityFactory::_removeEntity<component::EntityType::VEHICLE_TIRE_REAR>(common::EntityID entityID) {
            _removeEntity<component::EntityType::VEHICLE_TIRE_FRONT>(entityID);
        }

        template<>
        void EntityFactory::_removeEntity<component::EntityType::WALL>(common::EntityID entityID) {
            removeComponent<component::Texture>(entityID);
            removeComponent<component::Shape>(entityID);

            removePhysicalBody(entityID);
            removeComponent<component::PhysicalProperties>(entityID);

            removeTag<component::Tag_Static>(entityID);
            removeTag<component::Tag_TextureShaded>(entityID);
        }

        template<>
        void EntityFactory::_removeEntity<component::EntityType::SIMPLE_BULLET>(common::EntityID entityID) {
            removeComponent<component::Shape>(entityID);
            removeComponent<component::Placement>(entityID);
            removeComponent<component::Texture>(entityID);

            removePhysicalBody(entityID);
            removeComponent<component::PhysicalProperties>(entityID);
            removeComponent<component::Trail>(entityID);

            removeTag<component::Tag_Dynamic>(entityID);
            removeTag<component::Tag_TextureShaded>(entityID);
        }

        template<>
        void EntityFactory::_removeEntity<component::EntityType::SIMPLE_PARTICLE>(common::EntityID entityID) {
            removeComponent<component::Particle>(entityID);
            removeComponentSafe<component::Appearance>(entityID);
            removeComponentSafe<component::Texture>(entityID);
        }

        template<>
        void EntityFactory::_removeEntity<component::EntityType::TEXT>(common::EntityID entityID) {
            removeComponent<component::Text>(entityID);
            removeTag<component::Tag_Static>(entityID);
        }

        void EntityFactory::removePhysicalBody(common::EntityID entityID) {
            auto entityPhysicalProps = mManager->get<component::PhysicalProperties>(entityID);
            mPhysicsWorld.DestroyBody(entityPhysicalProps.body);
        }

        void EntityFactory::attach(common::EntityID parent,
                                   common::EntityID child,
                                   component::EntityType parentType,
                                   component::EntityType childType) {
            auto &transformChildren = mManager->get<component::TransformChildren>(parent);
            transformChildren.children.emplace_back(child);

            auto &attachment = mManager->get<component::EntityAttachment>(parent);
            attachment.entities.emplace_back(child);
            attachment.entityTypes.emplace_back(childType);

            auto &attachee = mManager->get<component::EntityAttachee>(child);
            attachee.entityID = parent;
            attachee.entityType = parentType;
        }
    }
}