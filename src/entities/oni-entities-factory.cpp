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
        EntityFactory::EntityFactory(
                entities::SimMode sMode,
                const math::ZLayerManager &zLayerManager,
                b2World &physicsWorld) :
                mZLayerManager(zLayerManager),
                mPhysicsWorld(physicsWorld),
                mSimMode(sMode) {
            switch (sMode) {
                case SimMode::CLIENT: {
                    mEntityOperationPolicy = entities::EntityOperationPolicy::client();
                    break;
                }
                case SimMode::SERVER: {
                    mEntityOperationPolicy = entities::EntityOperationPolicy::server();
                    break;
                }
                default: {
                    assert(false);
                    break;
                }
            }
            mRand = std::make_unique<math::Rand>(0);
            mRegistryManager = std::make_unique<entities::EntityManager>();
        }

        EntityManager &
        EntityFactory::getEntityManager() {
            return *mRegistryManager;
        }

        EntityManager &
        EntityFactory::getEntityManager() const {
            return *mRegistryManager;
        }

        common::EntityID
        EntityFactory::createEntity() {
            auto id = mRegistryManager->create();
            assignSimMode(id, mSimMode);
            return id;
        }

        void
        EntityFactory::removeEntity(common::EntityID id) {
            removeEntity(id, mEntityOperationPolicy);
        }

        void
        EntityFactory::removeEntity(common::EntityID id,
                                    const entities::EntityOperationPolicy &policy) {
            if (policy.safe && !mRegistryManager->valid(id)) {
                return;
            }

            auto entityType = mRegistryManager->get<entities::EntityType>(id);
            _removeEntity(id, entityType, policy);

            if (policy.track) {
                mRegistryManager->destroyAndTrack(id);
            } else {
                mRegistryManager->destroy(id);
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
                case entities::EntityType::SMOKE:
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
                                                                     const component::WorldP3D &pos,
                                                                     const math::vec2 &size,
                                                                     const component::Heading &heading,
                                                                     const std::string &textureID) {
            auto &carConfig = createComponent<component::CarConfig>(entityID);
            carConfig.cgToRear = size.x / 2;
            carConfig.cgToFront = size.x / 2;
            carConfig.halfWidth = size.y / 2;
            assert(size.x - carConfig.cgToFront - carConfig.cgToRear < 0.00001f);

            createComponent<component::WorldP3D>(entityID, pos.x, pos.y, pos.z);

            auto &h = createComponent<component::Heading>(entityID);
            h = heading;

            auto &s = createComponent<component::Scale>(entityID);
            s.x = size.x;
            s.y = size.y;

            auto &properties = createComponent<component::PhysicalProperties>(entityID);
            properties.physicalCategory = component::PhysicalCategory::RACE_CAR;
            properties.bodyType = component::BodyType::DYNAMIC;
            properties.highPrecision = true;

            // TODO: Does this make sense? Or should it be same as createComponent?
            auto *body = createPhysicalBody(pos, size, heading.value, properties);

            auto &texture = createComponent<component::Texture>(entityID);
            texture.path = textureID;

            auto &car = createComponent<component::Car>(entityID);
            car.position.x = pos.x;
            car.position.y = pos.y;
            car.applyConfiguration(carConfig);

            auto &shape = createComponent<component::Shape>(entityID);

            auto &carLap = createComponent<gameplay::CarLapInfo>(entityID);
            carLap.entityID = entityID;
            carLap.lap = 0;
            carLap.bestLapTimeS = 0;
            carLap.lapTimeS = 0;

            auto &soundTag = createComponent<component::SoundTag>(entityID);
            soundTag = component::SoundTag::ENGINE_IDLE;

            createComponent<component::TransformChildren>(entityID);
            createComponent<component::EntityAttachment>(entityID);
            createComponent<component::Emitter>(entityID);

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

            createComponent<component::WorldP3D>(entityID, pos.x, pos.y, pos.z);

            auto &h = createComponent<component::Heading>(entityID);
            h = heading;

            auto &s = createComponent<component::Scale>(entityID);
            s.x = size.x;
            s.y = size.y;

            auto *body = createPhysicalBody(pos, size, heading.value, properties);

            auto &texture = createComponent<component::Texture>(entityID);
            texture.path = textureID;

            auto &shape = createComponent<component::Shape>(entityID);

            assignTag<component::Tag_TextureShaded>(entityID);
            assignTag<component::Tag_Dynamic>(entityID);
        }

        template<>
        void
        EntityFactory::_createEntity<entities::EntityType::VEHICLE_GUN>(common::EntityID entityID,
                                                                        const component::WorldP3D &pos,
                                                                        const math::vec2 &size,
                                                                        const component::Heading &heading,
                                                                        const std::string &textureID) {
            createComponent<component::WorldP3D>(entityID, pos.x, pos.y, pos.z);

            auto &h = createComponent<component::Heading>(entityID);
            h = heading;

            auto &s = createComponent<component::Scale>(entityID);
            s.x = size.x;
            s.y = size.y;

            auto &texture = createComponent<component::Texture>(entityID);
            texture.path = textureID;

            auto &shape = createComponent<component::Shape>(entityID);

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
            createComponent<component::WorldP3D>(entityID, pos.x, pos.y, pos.z);

            auto &h = createComponent<component::Heading>(entityID);
            h = heading;

            auto &s = createComponent<component::Scale>(entityID);
            s.x = size.x;
            s.y = size.y;

            auto &texture = createComponent<component::Texture>(entityID);
            texture.path = textureID;

            auto &shape = createComponent<component::Shape>(entityID);

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
                                                                 const component::WorldP3D &pos,
                                                                 const math::vec2 &size,
                                                                 const component::Heading &heading,
                                                                 const std::string &textureID) {
            auto &properties = createComponent<component::PhysicalProperties>(entityID);
            properties.highPrecision = false;
            properties.bodyType = component::BodyType::STATIC;
            properties.physicalCategory = component::PhysicalCategory::WALL;
            auto *body = createPhysicalBody(pos, size, heading.value, properties);

            auto &shape = createComponent<component::Shape>(entityID);
            shape.setZ(pos.z);
            shape.setSize(size);
            math::localToWorldTranslation(pos, shape);

            auto &texture = createComponent<component::Texture>(entityID);
            texture.path = textureID;

            assignTag<component::Tag_Static>(entityID);
            assignTag<component::Tag_TextureShaded>(entityID);
        }

        template<>
        void
        EntityFactory::_createEntity<entities::EntityType::SIMPLE_SPRITE>(common::EntityID entityID,
                                                                          const component::WorldP3D &pos,
                                                                          const math::vec2 &size,
                                                                          const component::Heading &heading,
                                                                          const math::vec4 &color) {
            auto &shape = createComponent<component::Shape>(entityID);
            shape.setZ(pos.z);
            shape.setSize(size);
            math::localToWorldTranslation(pos, shape);

            auto &appearance = createComponent<component::Appearance>(entityID);
            appearance.color = color;

            assignTag<component::Tag_Static>(entityID);
            assignTag<component::Tag_ColorShaded>(entityID);
        }

        template<>
        void
        EntityFactory::_createEntity<entities::EntityType::SIMPLE_SPRITE>(common::EntityID entityID,
                                                                          const component::WorldP3D &pos,
                                                                          const math::vec2 &size,
                                                                          const component::Heading &heading,
                                                                          const std::string &textureID) {
            auto &shape = createComponent<component::Shape>(entityID);
            shape.setZ(pos.z);
            shape.setSize(size);
            math::localToWorldTranslation(pos, shape);

            auto &texture = createComponent<component::Texture>(entityID);
            texture.path = textureID;

            assignTag<component::Tag_Static>(entityID);
            assignTag<component::Tag_TextureShaded>(entityID);
        }

        template<>
        void
        EntityFactory::_createEntity<entities::EntityType::SIMPLE_PARTICLE>(common::EntityID entityID,
                                                                            const component::WorldP3D &pos,
                                                                            const math::vec4 &color,
                                                                            const common::r32 &halfSize,
                                                                            const bool &randomize) {
            createComponent<component::Size>(entityID, halfSize, halfSize);

            createComponent<component::WorldP3D>(entityID, pos.x, pos.y, pos.z);

            auto &h = createComponent<component::Heading>(entityID);

            auto &s = createComponent<component::Scale>(entityID);

            auto &appearance = createComponent<component::Appearance>(entityID);
            appearance.color = color;

            auto &velocity = createComponent<component::Velocity>(entityID);

            auto &age = createComponent<component::Age>(entityID);
            age.currentAge = 0.f;

            if (randomize) {
                h.value = mRand->next_r32(0, common::FULL_CIRCLE_IN_RAD);
                velocity.currentVelocity = mRand->next_r32(1.f, 7.f);
                age.maxAge = mRand->next_r32(0.2f, 1.f);
            }

            assignTag<component::Tag_Particle>(entityID);
        }

        template<>
        void
        EntityFactory::_createEntity<entities::EntityType::SIMPLE_PARTICLE>(common::EntityID entityID,
                                                                            const component::WorldP3D &pos,
                                                                            const std::string &textureID,
                                                                            const common::r32 &halfSize,
                                                                            const bool &randomize) {
            createComponent<component::Size>(entityID, halfSize, halfSize);

            createComponent<component::WorldP3D>(entityID, pos.x, pos.y, pos.z);

            auto &h = createComponent<component::Heading>(entityID);

            auto &s = createComponent<component::Scale>(entityID);

            auto &texture = createComponent<component::Texture>(entityID);
            texture.path = textureID;

            auto &velocity = createComponent<component::Velocity>(entityID);

            auto &age = createComponent<component::Age>(entityID);
            age.currentAge = 0.f;

            if (randomize) {
                h.value = mRand->next_r32(0, common::FULL_CIRCLE_IN_RAD);
                velocity.currentVelocity = mRand->next_r32(1.f, 7.f);
                age.maxAge = mRand->next_r32(0.2f, 1.f);
            }

            assignTag<component::Tag_Particle>(entityID);
        }

        template<>
        void
        EntityFactory::_createEntity<entities::EntityType::SIMPLE_BLAST_PARTICLE>(common::EntityID entityID,
                                                                                  const component::WorldP3D &pos,
                                                                                  const std::string &textureID,
                                                                                  const common::r32 &halfSize,
                                                                                  const bool &randomize) {
            createComponent<component::Size>(entityID, halfSize, halfSize);

            createComponent<component::WorldP3D>(entityID, pos.x, pos.y, pos.z);

            auto &h = createComponent<component::Heading>(entityID);

            auto &s = createComponent<component::Scale>(entityID);

//            auto &appearance = createComponent<component::Appearance>(entityID);
//            appearance.color = {.5f, .5f, .5f, 1.f};

            auto &texture = createComponent<component::Texture>(entityID);
            texture.path = textureID;

            auto &velocity = createComponent<component::Velocity>(entityID);

            auto &age = createComponent<component::Age>(entityID);
            age.currentAge = 0.f;

            if (randomize) {
                h.value = mRand->next_r32(0, common::FULL_CIRCLE_IN_RAD);
                velocity.currentVelocity = mRand->next_r32(1.f, 2.f);
                age.maxAge = mRand->next_r32(0.2f, 1.f);
            }

            assignTag<component::Tag_Particle>(entityID);
            assignTag<component::Tag_SplatOnDeath>(entityID);
        }

        template<>
        void
        EntityFactory::_createEntity<entities::EntityType::SIMPLE_ROCKET>(common::EntityID entityID,
                                                                          const component::WorldP3D &pos,
                                                                          const math::vec2 &size,
                                                                          const component::Heading &heading,
                                                                          const std::string &textureID,
                                                                          const common::r32 &velocity) {
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
                    b2Vec2(static_cast<common::r32>(cos(heading.value) * velocity),
                           static_cast<common::r32>(sin(heading.value) * velocity)),
                    true);
            body->ApplyAngularImpulse(1, true);

            auto &shape = createComponent<component::Shape>(entityID);

            createComponent<component::WorldP3D>(entityID, pos.x, pos.y, pos.z);

            auto &h = createComponent<component::Heading>(entityID);
            h = heading;

            auto &s = createComponent<component::Scale>(entityID);
            s.x = size.x;
            s.y = size.y;

            auto &trail = createComponent<component::Trail>(entityID);
            trail.previousPos.push_back(pos);
            trail.velocity.push_back(velocity);

            auto &texture = createComponent<component::Texture>(entityID);
            texture.path = textureID;

            auto &soundTag = createComponent<component::SoundTag>(entityID);
            soundTag = component::SoundTag::ROCKET;

            assignTag<component::Tag_Dynamic>(entityID);
            assignTag<component::Tag_TextureShaded>(entityID);
            assignTag<component::Tag_Audible>(entityID);
        }

        template<>
        void
        EntityFactory::_createEntity<entities::EntityType::TEXT>(common::EntityID entityID,
                                                                 const component::WorldP3D &pos,
                                                                 const std::string &text) {

            auto &textComponent = createComponent<component::Text>(entityID);
            textComponent.textContent = text;

            createComponent<component::WorldP3D>(entityID, pos.x, pos.y, pos.z);

            assignTag<component::Tag_Static>(entityID);
        }

        template<>
        void
        EntityFactory::_createEntity<entities::EntityType::WORLD_CHUNK>(common::EntityID entityID,
                                                                        const component::WorldP3D &pos,
                                                                        const math::vec2 &size,
                                                                        const component::Heading &heading,
                                                                        const std::string &textureID) {
            auto &shape = createComponent<component::Shape>(entityID);
            shape.setZ(pos.z);
            shape.setSize(size);
            math::localToWorldTranslation(pos, shape);

            auto &texture = createComponent<component::Texture>(entityID);
            texture.path = textureID;

            createComponent<level::Chunk>(entityID);

            assignTag<component::Tag_Static>(entityID);
            assignTag<component::Tag_TextureShaded>(entityID);
        }

        template<>
        void
        EntityFactory::_createEntity<entities::EntityType::WORLD_CHUNK>(common::EntityID entityID,
                                                                        const component::WorldP3D &pos,
                                                                        const math::vec2 &size,
                                                                        const component::Heading &heading,
                                                                        const math::vec4 &color) {
            auto &shape = createComponent<component::Shape>(entityID);
            shape.setZ(pos.z);
            shape.setSize(size);
            math::localToWorldTranslation(pos, shape);

            auto &appearance = createComponent<component::Appearance>(entityID);
            appearance.color = color;

            createComponent<level::Chunk>(entityID);

            assignTag<component::Tag_Static>(entityID);
            assignTag<component::Tag_ColorShaded>(entityID);
        }

        common::EntityID
        EntityFactory::createEntity_SmokeCloud() {
            auto id = createEntity();
            createComponent<component::WorldP3D>(id);
            createComponent<component::Texture>(id);
            createComponent<component::Shape>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Age>(id);
            createComponent<component::Velocity>(id);
            createComponent<entities::EntityType>(id, entities::EntityType::SMOKE);

            assignTag<component::Tag_TextureShaded>(id);
            assignTag<component::Tag_Dynamic>(id);

            return id;
        }

        b2Body *
        EntityFactory::createPhysicalBody(const component::WorldP3D &pos,
                                          const math::vec2 &size,
                                          common::r32 heading,
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
                        collisionSensor.filter.groupIndex = -static_cast<common::i16>(properties.physicalCategory);
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

        void
        EntityFactory::assignSimMode(common::EntityID id,
                                     entities::SimMode sMode) {
            switch (sMode) {
                case SimMode::CLIENT: {
                    createComponent<component::Tag_SimModeClient>(id);
                    break;
                }
                case SimMode::SERVER: {
                    createComponent<component::Tag_SimModeServer>(id);
                    break;
                }
                default: {
                    assert(false);
                }
            }
        }

        void
        EntityFactory::setTexture(common::EntityID id,
                                  std::string_view path) {
            mRegistryManager->get<component::Texture>(id).path = path;
        }

        void
        EntityFactory::setRandAge(common::EntityID id,
                                  common::i32 lower,
                                  common::i32 upper) {
            auto &age = mRegistryManager->get<component::Age>(id);
            age.maxAge = mRand->next_i32(lower, upper);
        }

        void
        EntityFactory::setRandHeading(common::EntityID id) {
            auto &heading = mRegistryManager->get<component::Heading>(id);
            heading.value = mRand->next_r32(0.f, common::FULL_CIRCLE_IN_RAD);
        }

        void
        EntityFactory::setRandVelocity(common::EntityID id,
                                       common::i32 lower,
                                       common::i32 upper) {
            auto &velocity = mRegistryManager->get<component::Velocity>(id);
            velocity.currentVelocity = mRand->next_r32(lower, upper);
            velocity.maxVelocity = velocity.currentVelocity;
        }

        void
        EntityFactory::setWorldP3D(common::EntityID id,
                                   common::r32 x,
                                   common::r32 y,
                                   common::r32 z) {
            auto &pos = mRegistryManager->get<component::WorldP3D>(id);
            pos.x = x;
            pos.y = y;
            pos.z = z;
        }

        void
        EntityFactory::setScale(common::EntityID id,
                                common::r32 x,
                                common::r32 y) {
            auto &scale = mRegistryManager->get<component::Scale>(id);
            scale.x = x;
            scale.y = y;
        }
    }
}