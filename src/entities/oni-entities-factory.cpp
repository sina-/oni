#include <oni-core/entities/oni-entities-factory.h>

#include <Box2D/Box2D.h>

#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-hierarchy.h>
#include <oni-core/component/oni-component-gameplay.h>
#include <oni-core/gameplay/oni-gameplay-lap-tracker.h>
#include <oni-core/level/oni-level-chunk.h>
#include <oni-core/math/oni-math-rand.h>
#include <oni-core/math/oni-math-transformation.h>

namespace oni {
    namespace entities {
        EntityFactory::EntityFactory(const math::ZLayerManager &zLayerManager,
                                     b2World &physicsWorld) :
                mZLayerManager(zLayerManager),
                mPhysicsWorld{physicsWorld} {
            mRand = std::make_unique<math::Rand>(0);
            mRegistryManager = std::make_unique<entities::EntityManager>();
        }

        EntityManager &
        EntityFactory::getEntityManager() {
            return *mRegistryManager;
        }

        common::EntityID
        EntityFactory::createEntity() {
            auto entityID = mRegistryManager->create();
            return entityID;
        }


        void
        EntityFactory::removeEntity(common::EntityID entityID,
                                    const entities::EntityOperationPolicy &policy) {
            if (policy.safe && !mRegistryManager->valid(entityID)) {
                return;
            }

            auto entityType = mRegistryManager->get<entities::EntityType>(entityID);
            _removeEntity(entityID, entityType, policy);

            if (policy.track) {
                mRegistryManager->destroyAndTrack(entityID);
            } else {
                mRegistryManager->destroy(entityID);
            }
        }

        void
        EntityFactory::_removeEntity(common::EntityID entityID,
                                     entities::EntityType entityType,
                                     const entities::EntityOperationPolicy &policy) {
            // TODO: I hate this, for every new type and every change to old types I have to remember to add, or update,
            // the deletion procedure. Technically dtor would be the right solution, then I can remove all this code
            // and only depend on registry->destroy() method calling the dtor.
            switch (entityType) {
                case entities::EntityType::RACE_CAR: {
                    _removeEntity<entities::EntityType::RACE_CAR>(entityID, policy);
                    break;
                }
                case entities::EntityType::WALL: {
                    _removeEntity<entities::EntityType::WALL>(entityID, policy);
                    break;
                }
                case entities::EntityType::SIMPLE_ROCKET: {
                    _removeEntity<entities::EntityType::SIMPLE_ROCKET>(entityID, policy);
                    break;
                }
                case entities::EntityType::VEHICLE_GUN:
                case entities::EntityType::VEHICLE_TIRE_FRONT:
                case entities::EntityType::VEHICLE_TIRE_REAR:
                case entities::EntityType::ROAD:
                case entities::EntityType::VEHICLE:
                case entities::EntityType::BACKGROUND:
                case entities::EntityType::CANVAS:
                case entities::EntityType::UI:
                case entities::EntityType::SIMPLE_SPRITE:
                case entities::EntityType::SIMPLE_PARTICLE:
                case entities::EntityType::SIMPLE_BLAST_PARTICLE:
                case entities::EntityType::TEXT:
                case entities::EntityType::WORLD_CHUNK: {
                    break;
                }
                case entities::EntityType::LAST:
                case entities::EntityType::UNKNOWN:
                default: {
                    assert(false);
                    break;
                }
            }
        }

        template<>
        void
        EntityFactory::_createEntity<entities::EntityType::RACE_CAR>(common::EntityID entityID,
                                                                     const component::WorldP3D &worldPos,
                                                                     const math::vec2 &size,
                                                                     const component::Heading &heading,
                                                                     const std::string &textureID) {
            auto &carConfig = createComponent<component::CarConfig>(entityID);
            carConfig.cgToRear = size.x / 2;
            carConfig.cgToFront = size.x / 2;
            carConfig.halfWidth = size.y / 2;
            assert(size.x - carConfig.cgToFront - carConfig.cgToRear < 0.00001f);

            auto &p = createComponent<component::WorldP3D>(entityID);
            p = worldPos;

            auto &h = createComponent<component::Heading>(entityID);
            h = heading;

            auto &s = createComponent<component::Scale>(entityID);

            auto &properties = createComponent<component::PhysicalProperties>(entityID);
            properties.physicalCategory = component::PhysicalCategory::RACE_CAR;
            properties.bodyType = component::BodyType::DYNAMIC;
            properties.highPrecision = true;

            // TODO: Does this make sense? Or should it be same as createComponent?
            auto *body = createPhysicalBody(worldPos, size, heading.value, properties);

            auto &texture = createComponent<component::Texture>(entityID);
            texture.filePath = textureID;
            texture.status = component::TextureStatus::NEEDS_LOADING_USING_PATH;

            auto &car = createComponent<component::Car>(entityID);
            car.position.x = worldPos.x;
            car.position.y = worldPos.y;
            car.applyConfiguration(carConfig);

            auto &shape = createComponent<component::Shape>(entityID);
            shape.setZ(worldPos.z);
            shape.setSizeFromOrigin(size);
            shape.centerAlign();

            auto &carLap = createComponent<gameplay::CarLapInfo>(entityID);
            carLap.entityID = entityID;
            carLap.lap = 0;
            carLap.bestLapTimeS = 0;
            carLap.lapTimeS = 0;

            auto &soundTag = createComponent<component::SoundTag>(entityID);
            soundTag = component::SoundTag::ENGINE_IDLE;

            createComponent<component::TransformChildren>(entityID);
            createComponent<component::EntityAttachment>(entityID);

            assignTag<component::Tag_TextureShaded>(entityID);
            assignTag<component::Tag_Dynamic>(entityID);
            assignTag<component::Tag_Audible>(entityID);
        }

        template<>
        void
        EntityFactory::_createEntity<entities::EntityType::VEHICLE>(common::EntityID entityID,
                                                                    const component::WorldP3D &pos,
                                                                    const math::vec2 &size,
                                                                    const component::Heading &heading,
                                                                    const std::string &textureID) {
            auto &properties = createComponent<component::PhysicalProperties>(entityID);
            properties.friction = 1.f;
            properties.density = 0.1f;
            properties.angularDamping = 2.f;
            properties.linearDamping = 2.f;
            properties.highPrecision = false;
            properties.bodyType = component::BodyType::DYNAMIC;
            properties.physicalCategory = component::PhysicalCategory::VEHICLE;

            auto &p = createComponent<component::WorldP3D>(entityID);
            p = pos;

            auto &h = createComponent<component::Heading>(entityID);
            h = heading;

            auto &s = createComponent<component::Scale>(entityID);

            auto *body = createPhysicalBody(pos, size, heading.value, properties);

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
        void
        EntityFactory::_createEntity<entities::EntityType::VEHICLE_GUN>(common::EntityID entityID,
                                                                        const component::WorldP3D &worldPos,
                                                                        const math::vec2 &size,
                                                                        const component::Heading &heading,
                                                                        const std::string &textureID) {
            auto &p = createComponent<component::WorldP3D>(entityID);
            p = worldPos;

            auto &h = createComponent<component::Heading>(entityID);
            h = heading;

            auto &s = createComponent<component::Scale>(entityID);

            auto &texture = createComponent<component::Texture>(entityID);
            texture.filePath = textureID;
            texture.status = component::TextureStatus::NEEDS_LOADING_USING_PATH;

            auto &shape = createComponent<component::Shape>(entityID);
            shape.setZ(worldPos.z);
            shape.setSizeFromOrigin(size);
            shape.centerAlign();

            createComponent<component::EntityAttachee>(entityID);
            createComponent<component::TransformParent>(entityID);
            createComponent<component::GunCoolDown>(entityID);

            assignTag<component::Tag_Dynamic>(entityID);
            assignTag<component::Tag_TextureShaded>(entityID);
        }

        template<>
        void
        EntityFactory::_createEntity<entities::EntityType::VEHICLE_TIRE_FRONT>(common::EntityID entityID,
                                                                               const component::WorldP3D &pos,
                                                                               const math::vec2 &size,
                                                                               const component::Heading &heading,
                                                                               const std::string &textureID) {
            auto &p = createComponent<component::WorldP3D>(entityID);
            p = pos;

            auto &h = createComponent<component::Heading>(entityID);
            h = heading;

            auto &s = createComponent<component::Scale>(entityID);

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
        void
        EntityFactory::_createEntity<entities::EntityType::VEHICLE_TIRE_REAR>(common::EntityID entityID,
                                                                              const component::WorldP3D &pos,
                                                                              const math::vec2 &size,
                                                                              const component::Heading &heading,
                                                                              const std::string &textureID) {
            _createEntity<entities::EntityType::VEHICLE_TIRE_FRONT>(entityID, pos, size, heading, textureID);
        }

        template<>
        void
        EntityFactory::_createEntity<entities::EntityType::WALL>(common::EntityID entityID,
                                                                 const component::WorldP3D &worldPos,
                                                                 const math::vec2 &size,
                                                                 const component::Heading &heading,
                                                                 const std::string &textureID) {
            auto &properties = createComponent<component::PhysicalProperties>(entityID);
            properties.highPrecision = false;
            properties.bodyType = component::BodyType::STATIC;
            properties.physicalCategory = component::PhysicalCategory::WALL;
            auto *body = createPhysicalBody(worldPos, size, heading.value, properties);

            auto &shape = createComponent<component::Shape>(entityID);
            shape.setZ(worldPos.z);
            shape.setSizeFromOrigin(size);
            math::localToWorldTranslation(worldPos, shape);

            auto &texture = createComponent<component::Texture>(entityID);
            texture.filePath = textureID;
            texture.status = component::TextureStatus::NEEDS_LOADING_USING_PATH;

            assignTag<component::Tag_Static>(entityID);
            assignTag<component::Tag_TextureShaded>(entityID);
        }

        template<>
        void
        EntityFactory::_createEntity<entities::EntityType::SIMPLE_SPRITE>(common::EntityID entityID,
                                                                          const component::WorldP3D &worldPos,
                                                                          const math::vec2 &size,
                                                                          const component::Heading &heading,
                                                                          const math::vec4 &color) {
            auto &shape = createComponent<component::Shape>(entityID);
            shape.setZ(worldPos.z);
            shape.setSizeFromOrigin(size);
            math::localToWorldTranslation(worldPos, shape);

            auto &appearance = createComponent<component::Appearance>(entityID);
            appearance.color = color;

            assignTag<component::Tag_Static>(entityID);
            assignTag<component::Tag_ColorShaded>(entityID);
        }

        template<>
        void
        EntityFactory::_createEntity<entities::EntityType::SIMPLE_SPRITE>(common::EntityID entityID,
                                                                          const component::WorldP3D &worldPos,
                                                                          const math::vec2 &size,
                                                                          const component::Heading &heading,
                                                                          const std::string &textureID) {
            auto &shape = createComponent<component::Shape>(entityID);
            shape.setZ(worldPos.z);
            shape.setSizeFromOrigin(size);
            math::localToWorldTranslation(worldPos, shape);

            auto &texture = createComponent<component::Texture>(entityID);
            texture.filePath = textureID;
            texture.status = component::TextureStatus::NEEDS_LOADING_USING_PATH;

            assignTag<component::Tag_Static>(entityID);
            assignTag<component::Tag_TextureShaded>(entityID);
        }

        template<>
        void
        EntityFactory::_createEntity<entities::EntityType::SIMPLE_PARTICLE>(common::EntityID entityID,
                                                                            const component::WorldP3D &pos,
                                                                            const math::vec4 &color,
                                                                            const common::real32 &halfSize,
                                                                            const bool &randomize) {
            auto &tessellation = createComponent<component::Tessellation>(entityID);
            tessellation.halfSize = halfSize;

            auto &p = createComponent<component::WorldP3D>(entityID);
            p = pos;

            auto &h = createComponent<component::Heading>(entityID);

            auto &s = createComponent<component::Scale>(entityID);

            auto &appearance = createComponent<component::Appearance>(entityID);
            appearance.color = color;

            auto &velocity = createComponent<component::Velocity>(entityID);

            auto &age = createComponent<component::Age>(entityID);
            age.currentAge = 0.f;

            if (randomize) {
                h.value = mRand->nextReal32(0, common::FULL_CIRCLE_IN_RAD);
                velocity.currentVelocity = mRand->nextReal32(1.f, 7.f);
                age.maxAge = mRand->nextReal32(0.2f, 1.f);
            }

            assignTag<component::Tag_Particle>(entityID);
            assignTag<component::Tag_ShaderOnlyParticlePhysics>(entityID);
        }

        template<>
        void
        EntityFactory::_createEntity<entities::EntityType::SIMPLE_PARTICLE>(common::EntityID entityID,
                                                                            const component::WorldP3D &pos,
                                                                            const std::string &textureID,
                                                                            const common::real32 &halfSize,
                                                                            const bool &randomize) {
            auto &tessellation = createComponent<component::Tessellation>(entityID);
            tessellation.halfSize = halfSize;

            auto &p = createComponent<component::WorldP3D>(entityID);
            p = pos;

            auto &h = createComponent<component::Heading>(entityID);

            auto &s = createComponent<component::Scale>(entityID);

            auto &texture = createComponent<component::Texture>(entityID);
            texture.filePath = textureID;
            texture.status = component::TextureStatus::NEEDS_LOADING_USING_PATH;

            auto &velocity = createComponent<component::Velocity>(entityID);

            auto &age = createComponent<component::Age>(entityID);
            age.currentAge = 0.f;

            if (randomize) {
                h.value = mRand->nextReal32(0, common::FULL_CIRCLE_IN_RAD);
                velocity.currentVelocity = mRand->nextReal32(1.f, 7.f);
                age.maxAge = mRand->nextReal32(0.2f, 1.f);
            }

            assignTag<component::Tag_Particle>(entityID);
            assignTag<component::Tag_ShaderOnlyParticlePhysics>(entityID);
        }

        template<>
        void
        EntityFactory::_createEntity<entities::EntityType::SIMPLE_BLAST_PARTICLE>(common::EntityID entityID,
                                                                                  const component::WorldP3D &pos,
                                                                                  const std::string &textureID,
                                                                                  const common::real32 &halfSize,
                                                                                  const bool &randomize) {
            auto &tessellation = createComponent<component::Tessellation>(entityID);
            tessellation.halfSize = halfSize;

            auto &p = createComponent<component::WorldP3D>(entityID);
            p = pos;

            auto &h = createComponent<component::Heading>(entityID);

            auto &s = createComponent<component::Scale>(entityID);

//            auto &appearance = createComponent<component::Appearance>(entityID);
//            appearance.color = {.5f, .5f, .5f, 1.f};

            auto &texture = createComponent<component::Texture>(entityID);
            texture.filePath = textureID;
            texture.status = component::TextureStatus::NEEDS_LOADING_USING_PATH;

            auto &velocity = createComponent<component::Velocity>(entityID);

            auto &age = createComponent<component::Age>(entityID);
            age.currentAge = 0.f;

            if (randomize) {
                h.value = mRand->nextReal32(0, common::FULL_CIRCLE_IN_RAD);
                velocity.currentVelocity = mRand->nextReal32(1.f, 7.f);
                age.maxAge = mRand->nextReal32(0.2f, 1.f);
            }

            assignTag<component::Tag_Particle>(entityID);
            assignTag<component::Tag_ShaderOnlyParticlePhysics>(entityID);
            assignTag<component::Tag_SplatOnDeath>(entityID);
        }

        template<>
        void
        EntityFactory::_createEntity<entities::EntityType::SIMPLE_ROCKET>(common::EntityID entityID,
                                                                          const component::WorldP3D &pos,
                                                                          const math::vec2 &size,
                                                                          const component::Heading &heading,
                                                                          const std::string &textureID,
                                                                          const common::real32 &velocity) {
            auto &properties = createComponent<component::PhysicalProperties>(entityID);
            properties.friction = 1.f;
            properties.density = 0.1f;
            properties.angularDamping = 2.f;
            properties.linearDamping = 0.1f;
            properties.highPrecision = true;
            properties.bodyType = component::BodyType::DYNAMIC;
            properties.physicalCategory = component::PhysicalCategory::ROCKET;
            properties.collisionWithinCategory = false;

            auto *body = createPhysicalBody(pos, size, heading.value, properties);

            body->ApplyForceToCenter(
                    b2Vec2(static_cast<common::real32>(cos(heading.value) * velocity),
                           static_cast<common::real32>(sin(heading.value) * velocity)),
                    true);

            auto &shape = createComponent<component::Shape>(entityID);
            shape.setZ(pos.z);
            shape.setSizeFromOrigin(size);
            shape.centerAlign();

            auto &p = createComponent<component::WorldP3D>(entityID);
            p = pos;

            auto &h = createComponent<component::Heading>(entityID);
            h = heading;

            auto &s = createComponent<component::Scale>(entityID);

            auto &trail = createComponent<component::Trail>(entityID);
            trail.previousPos.push_back(pos);
            trail.velocity.push_back(velocity);

            auto &texture = createComponent<component::Texture>(entityID);
            texture.filePath = textureID;
            texture.status = component::TextureStatus::NEEDS_LOADING_USING_PATH;

            auto &soundTag = createComponent<component::SoundTag>(entityID);
            soundTag = component::SoundTag::ROCKET;

            assignTag<component::Tag_Dynamic>(entityID);
            assignTag<component::Tag_TextureShaded>(entityID);
            assignTag<component::Tag_Audible>(entityID);
        }

        template<>
        void
        EntityFactory::_createEntity<entities::EntityType::TEXT>(common::EntityID entityID,
                                                                 const component::WorldP3D &worldPos,
                                                                 const std::string &text) {

            auto &textComponent = createComponent<component::Text>(entityID);
            textComponent.textContent = text;

            auto &pos = createComponent<component::WorldP3D>(entityID);
            pos = worldPos;

            assignTag<component::Tag_Static>(entityID);
        }

        template<>
        void
        EntityFactory::_createEntity<entities::EntityType::WORLD_CHUNK>(common::EntityID entityID,
                                                                        const component::WorldP3D &worldPos,
                                                                        const math::vec2 &size,
                                                                        const component::Heading &heading,
                                                                        const std::string &textureID) {
            auto &shape = createComponent<component::Shape>(entityID);
            shape.setZ(worldPos.z);
            shape.setSizeFromOrigin(size);
            math::localToWorldTranslation(worldPos, shape);

            auto &pos = createComponent<component::WorldP3D>(entityID);
            pos = worldPos;

            auto &texture = createComponent<component::Texture>(entityID);
            texture.filePath = textureID;
            texture.status = component::TextureStatus::NEEDS_LOADING_USING_PATH;

            createComponent<level::Chunk>(entityID);

            assignTag<component::Tag_Static>(entityID);
            assignTag<component::Tag_TextureShaded>(entityID);
        }

        template<>
        void
        EntityFactory::_createEntity<entities::EntityType::WORLD_CHUNK>(common::EntityID entityID,
                                                                        const component::WorldP3D &worldPos,
                                                                        const math::vec2 &size,
                                                                        const component::Heading &heading,
                                                                        const math::vec4 &color) {
            auto &shape = createComponent<component::Shape>(entityID);
            shape.setZ(worldPos.z);
            shape.setSizeFromOrigin(size);
            math::localToWorldTranslation(worldPos, shape);

            auto &appearance = createComponent<component::Appearance>(entityID);
            appearance.color = color;

            createComponent<level::Chunk>(entityID);

            assignTag<component::Tag_Static>(entityID);
            assignTag<component::Tag_ColorShaded>(entityID);
        }

        b2Body *
        EntityFactory::createPhysicalBody(const component::WorldP3D &worldPos,
                                          const math::vec2 &size,
                                          common::real32 heading,
                                          component::PhysicalProperties &properties) {
            b2PolygonShape shape{};
            shape.SetAsBox(size.x / 2.0f, size.y / 2.0f);

            auto &pos = worldPos.value;

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
                    bodyDef.position.x = pos.x;
                    bodyDef.position.y = pos.y;
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
                    bodyDef.position.x = pos.x + size.x / 2.0f;
                    bodyDef.position.y = pos.y + size.y / 2.0f;
                    bodyDef.type = b2_staticBody;
                    body = mPhysicsWorld.CreateBody(&bodyDef);
                    body->CreateFixture(&shape, 0.f);
                    break;
                }
                case component::BodyType::KINEMATIC: {
                    bodyDef.position.x = pos.x;
                    bodyDef.position.y = pos.y;
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
        void
        EntityFactory::_removeEntity<entities::EntityType::RACE_CAR>(common::EntityID entityID,
                                                                     const entities::EntityOperationPolicy &policy) {
            // TODO: When notifying clients of this, the texture in memory should be evicted.


            // TODO: For some reason this is the wrong list of entities, check valid() call returning false,
            //  on client side even though during serialization I tell entt to keep mapping these ids to client side ids
            auto &attachments = mRegistryManager->get<component::EntityAttachment>(entityID);
            for (common::size i = 0; i < attachments.entities.size(); ++i) {
                auto attachmentType = attachments.entityTypes[i];
                auto attachmentEntityID = attachments.entities[i];
                removeEntity(attachmentEntityID, policy);
            }

            removePhysicalBody(entityID);
        }

        template<>
        void
        EntityFactory::_removeEntity<entities::EntityType::WALL>(common::EntityID entityID,
                                                                 const entities::EntityOperationPolicy &policy) {
            removePhysicalBody(entityID);
        }

        template<>
        void
        EntityFactory::_removeEntity<entities::EntityType::SIMPLE_ROCKET>(common::EntityID entityID,
                                                                          const entities::EntityOperationPolicy &policy) {
            removePhysicalBody(entityID);
        }

        void
        EntityFactory::removePhysicalBody(common::EntityID entityID) {
            // TODO: This is server side only at the moment, so deletion on client side would fail, if I add client
            // side prediction I can remove this check
            if (mRegistryManager->has<component::PhysicalProperties>(entityID)) {
                auto entityPhysicalProps = mRegistryManager->get<component::PhysicalProperties>(entityID);
                mPhysicsWorld.DestroyBody(entityPhysicalProps.body);
            }
        }

        void
        EntityFactory::attach(common::EntityID parent,
                              common::EntityID child,
                              entities::EntityType parentType,
                              entities::EntityType childType) {
            auto &transformChildren = mRegistryManager->get<component::TransformChildren>(parent);
            transformChildren.children.emplace_back(child);

            auto &attachment = mRegistryManager->get<component::EntityAttachment>(parent);
            attachment.entities.emplace_back(child);
            attachment.entityTypes.emplace_back(childType);

            auto &attachee = mRegistryManager->get<component::EntityAttachee>(child);
            attachee.entityID = parent;
            attachee.entityType = parentType;
        }

        void
        EntityFactory::tagForNetworkSync(common::EntityID entityID) {
            assignTag<component::Tag_RequiresNetworkSync>(entityID);
        }
    }
}