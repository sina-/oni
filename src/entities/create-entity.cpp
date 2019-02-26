#include <oni-core/entities/create-entity.h>

#include <Box2D/Box2D.h>

#include <oni-core/component/geometry.h>
#include <oni-core/component/hierarchy.h>

namespace oni {
    namespace entities {

        EntityFactory::EntityFactory(EntityManager &manager,
                                     const math::ZLayerManager &zLayerManager,
                                     b2World &physicsWorld) :
                mManager(manager),
                mZLayerManager(zLayerManager),
                mPhysicsWorld{physicsWorld} {
        }

        common::EntityID EntityFactory::createEntity(bool tagNew) {
            auto entityID = mManager.create();
            if (tagNew) {
                mManager.assign<component::Tag_NewEntity>(entityID);
            }
            return entityID;
        }

        void EntityFactory::destroyEntity(common::EntityID entityID) {
            if (!mManager.has<component::Tag_NewEntity>(entityID)) {
                mManager.addDeletedEntity(entityID);
            }
            mManager.destroy(entityID);
        }

        // TODO: Why can't I just call removeEntity<entityType>(entityID)?
        void EntityFactory::removeEntity(common::EntityID entityID, component::EntityType entityType) {
            switch (entityType) {
                case component::EntityType::RACE_CAR: {
                    removeEntity<component::EntityType::RACE_CAR>(entityID);
                    break;
                }
                case component::EntityType::WALL: {
                    removeEntity<component::EntityType::WALL>(entityID);
                    break;
                }
                case component::EntityType::VEHICLE_GUN: {
                    removeEntity<component::EntityType::VEHICLE_GUN>(entityID);
                    break;
                }
                case component::EntityType::VEHICLE_TIRE: {
                    removeEntity<component::EntityType::VEHICLE_TIRE>(entityID);
                    break;
                }
                case component::EntityType::ROAD:
                case component::EntityType::VEHICLE:
                case component::EntityType::BACKGROUND:
                case component::EntityType::SKID_LINE:
                case component::EntityType::UI:
                case component::EntityType::UNKNOWN: {
                    assert(false);
                    break;
                }
            }
        }

        template<>
        void EntityFactory::createEntity<component::EntityType::RACE_CAR>(
                common::EntityID entityID,
                math::vec3 &pos,
                math::vec2 &size,
                common::real32 &heading,
                std::string &textureID) {
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

            createComponent<component::TransformChildren>(entityID);
            createComponent<component::EntityAttachment>(entityID);

            assignTag<component::Tag_TextureShaded>(entityID);
            assignTag<component::Tag_Dynamic>(entityID);
        }


        template<>
        void EntityFactory::removeEntity<component::EntityType::RACE_CAR>(common::EntityID entityID) {
            auto &attachments = mManager.get<component::EntityAttachment>(entityID);
            for (common::size i = 0; i < attachments.entities.size(); ++i) {
                component::EntityType attachmentType = attachments.entityTypes[i];
                removeEntity(entityID, attachmentType);
            }
            removeComponent<component::EntityAttachment>(entityID);
            removeComponent<component::Shape>(entityID);
            removeComponent<component::Placement>(entityID);
            // TODO: When notifying clients of this, the texture in memory should be evicted.
            removeComponent<component::Texture>(entityID);
            removeComponent<component::TransformChildren>(entityID);
            removeComponent<component::Car>(entityID);
            removeComponent<component::CarConfig>(entityID);

            removePhysicalBody(entityID);
            removeComponent<component::PhysicalProperties>(entityID);

            removeTag<component::Tag_Dynamic>(entityID);
            removeTag<component::Tag_TextureShaded>(entityID);
        }

        template<>
        void EntityFactory::createEntity<component::EntityType::VEHICLE_GUN>(
                common::EntityID entityID,
                oni::math::vec3 &pos,
                oni::math::vec2 &size,
                common::real32 &heading,
                std::string &textureID) {
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
        void EntityFactory::removeEntity<component::EntityType::VEHICLE_GUN>(common::EntityID entityID) {
            removeComponent<component::Placement>(entityID);
            removeComponent<component::Texture>(entityID);
            removeComponent<component::Shape>(entityID);
            removeComponent<component::EntityAttachee>(entityID);
            removeComponent<component::TransformParent>(entityID);

            removeTag<component::Tag_Dynamic>(entityID);
            removeTag<component::Tag_TextureShaded>(entityID);
        }

        template<>
        void EntityFactory::createEntity<component::EntityType::VEHICLE_TIRE>(
                common::EntityID entityID,
                math::vec3 &pos,
                math::vec2 &size,
                common::real32 &heading,
                std::string &textureID) {
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
        void EntityFactory::removeEntity<component::EntityType::VEHICLE_TIRE>(common::EntityID entityID) {
            removeComponent<component::Placement>(entityID);
            removeComponent<component::Texture>(entityID);
            removeComponent<component::Shape>(entityID);
            removeComponent<component::EntityAttachee>(entityID);
            removeComponent<component::TransformParent>(entityID);

            removeTag<component::Tag_Dynamic>(entityID);
            removeTag<component::Tag_TextureShaded>(entityID);
        }

        template<>
        void EntityFactory::createEntity<component::EntityType::WALL>(
                common::EntityID entityID,
                math::vec3 &worldPos,
                math::vec2 &size,
                common::real32 &heading,
                std::string &textureID) {
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
        void EntityFactory::removeEntity<component::EntityType::WALL>(common::EntityID entityID) {
            removeComponent<component::Texture>(entityID);
            removeComponent<component::Shape>(entityID);

            removePhysicalBody(entityID);
            removeComponent<component::PhysicalProperties>(entityID);

            removeTag<component::Tag_Static>(entityID);
            removeTag<component::Tag_TextureShaded>(entityID);
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

        void EntityFactory::removePhysicalBody(common::EntityID entityID) {
            auto entityPhysicalProps = mManager.get<component::PhysicalProperties>(entityID);
            mPhysicsWorld.DestroyBody(entityPhysicalProps.body);
        }

        std::unique_lock<std::mutex> EntityFactory::scopedLock() {
            return mManager.scopedLock();
        }

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
            math::Transformation::shapeTransformation(
                    math::mat4::rotation(math::toRadians(angle), math::vec3{0.f, 0.f, 1.f}), sprite);
            manager.assign<component::Shape>(entityID, sprite);
        }

        void assignShapeWorld(EntityManager &manager, common::EntityID entityID, const math::vec2 &size,
                              const math::vec3 &worldPos) {
            auto sizeWithZ = math::vec3{size.x, size.y, worldPos.z};
            auto sprite = component::Shape::fromSizeAndRotation(sizeWithZ, 0);

            math::Transformation::localToWorldTranslation(worldPos, sprite);
            manager.assign<component::Shape>(entityID, sprite);
        }

        void assignPoint(EntityManager &manager,
                         common::EntityID entityID,
                         const math::vec3 &pos) {
            manager.assign<component::Point>(entityID, pos);
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
                case component::BodyType::UNKNOWN: {
                    assert("Unknown BodyType" && false);
                    break;
                }
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

        void assignParticle(EntityManager &manager, common::EntityID entityID, const math::vec3 &worldPos,
                            common::real32 heading, common::real32 age, common::real32 maxAge,
                            common::real32 velocity) {
            manager.assign<component::Particle>(entityID, age, maxAge, worldPos, heading, velocity);
        }

        void removeParticle(EntityManager &manager, common::EntityID entityID) {
            manager.remove<component::Particle>(entityID);
        }

        void removeShape(EntityManager &manager, common::EntityID entityID) {
            manager.remove<component::Shape>(entityID);
        }

        void removePlacement(EntityManager &manager, common::EntityID entityID) {
            manager.remove<component::Placement>(entityID);
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

        void attach(
                EntityManager &manager,
                common::EntityID parent,
                common::EntityID child,
                component::EntityType parentType,
                component::EntityType childType) {
            auto &transformChildren = manager.get<component::TransformChildren>(parent);
            transformChildren.children.emplace_back(child);

            auto &attachment = manager.get<component::EntityAttachment>(parent);
            attachment.entities.emplace_back(child);
            attachment.entityTypes.emplace_back(childType);

            auto &attachee = manager.get<component::EntityAttachee>(child);
            attachee.entityID = parent;
            attachee.entityType = parentType;
        }
    }
}