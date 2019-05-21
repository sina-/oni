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
        EntityFactory::createEntity(entities::EntityType type) {
            auto id = mRegistryManager->create();
            assignSimMode(id, mSimMode);
            createComponent<entities::EntityType>(id, type);
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
                case entities::EntityType::COMPLEMENT:
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
        EntityFactory::_createEntity<entities::EntityType::SIMPLE_BLAST_PARTICLE>(common::EntityID entityID,
                                                                                  const component::WorldP3D &pos,
                                                                                  const std::string &textureID,
                                                                                  const common::r32 &halfSize,
                                                                                  const bool &randomize) {
            createComponent<component::Scale>(entityID, halfSize, halfSize);

            createComponent<component::WorldP3D>(entityID, pos.x, pos.y, pos.z);

            auto &h = createComponent<component::Heading>(entityID);

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

        void
        EntityFactory::createPhysics(common::EntityID id,
                                     const component::WorldP3D &pos,
                                     const math::vec2 &size,
                                     common::r32 heading) {
            auto &props = mRegistryManager->get<component::PhysicalProperties>(id);
            auto shape = b2PolygonShape{};
            shape.SetAsBox(size.x / 2.0f, size.y / 2.0f);

            // NOTE: This is non-owning pointer. physicsWorld owns it.
            b2Body *body{};

            b2BodyDef bodyDef;
            bodyDef.bullet = props.highPrecision;
            bodyDef.angle = heading;
            bodyDef.linearDamping = props.linearDamping;
            bodyDef.angularDamping = props.angularDamping;

            b2FixtureDef fixtureDef;
            // NOTE: Box2D will create a copy of the shape, so it is safe to pass a local ref.
            fixtureDef.shape = &shape;
            fixtureDef.density = props.density;
            fixtureDef.friction = props.friction;

            switch (props.bodyType) {
                case component::BodyType::DYNAMIC : {
                    bodyDef.position.x = pos.x;
                    bodyDef.position.y = pos.y;
                    bodyDef.type = b2_dynamicBody;
                    body = mPhysicsWorld.CreateBody(&bodyDef);

                    b2FixtureDef collisionSensor;
                    collisionSensor.isSensor = true;
                    collisionSensor.shape = &shape;
                    collisionSensor.density = props.density;
                    collisionSensor.friction = props.friction;

                    if (!props.collisionWithinCategory) {
                        collisionSensor.filter.groupIndex = -static_cast<common::i16>(props.physicalCategory);
                    }

                    body->CreateFixture(&fixtureDef);
                    body->CreateFixture(&collisionSensor);
                    break;
                }
                case component::BodyType::STATIC: {
                    bodyDef.position.x = pos.x;
                    bodyDef.position.y = pos.y;
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
            props.body = body;
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
            if (mRegistryManager->has<component::Tag_Static>(id)) {
                auto &shape = mRegistryManager->get<component::Shape>(id);
                shape.setZ(z);
                math::localToWorldTranslation(x, y, shape);
            } else {
                auto &pos = mRegistryManager->get<component::WorldP3D>(id);
                pos.x = x;
                pos.y = y;
                pos.z = z;
            }
        }

        void
        EntityFactory::setScale(common::EntityID id,
                                common::r32 x,
                                common::r32 y) {
            if (mRegistryManager->has<component::Tag_Static>(id)) {
                auto &shape = mRegistryManager->get<component::Shape>(id);
                shape.setSize(x, y);
            } else {
                auto &scale = mRegistryManager->get<component::Scale>(id);
                scale.x = x;
                scale.y = y;
            }
        }

        void
        EntityFactory::setHeading(common::EntityID id,
                                  common::r32 heading) {
            auto &h = mRegistryManager->get<component::Heading>(id);
            h.value = heading;
        }

        void
        EntityFactory::tagForRemoval(common::EntityID id) {
            mEntitiesToDelete.push_back(id);
            mEntitiesToDeletePolicy.push_back(mEntityOperationPolicy);
        }

        void
        EntityFactory::tagForRemoval(common::EntityID id,
                                     const entities::EntityOperationPolicy &policy) {
            mEntitiesToDelete.push_back(id);
            mEntitiesToDeletePolicy.push_back(policy);
        }

        void
        EntityFactory::flushEntityRemovals() {
            for (common::size i = 0; i < mEntitiesToDelete.size(); ++i) {
                removeEntity(mEntitiesToDelete[i], mEntitiesToDeletePolicy[i]);
            }

            mEntitiesToDelete.clear();
            mEntitiesToDeletePolicy.clear();
        }

        common::EntityID
        EntityFactory::createEntity_SmokeCloud() {
            auto id = createEntity(entities::EntityType::SMOKE);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Texture>(id);
            createComponent<component::Shape>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Age>(id);
            createComponent<component::Velocity>(id);

            assignTag<component::Tag_TextureShaded>(id);
            assignTag<component::Tag_Dynamic>(id);

            return id;
        }

        common::EntityID
        EntityFactory::createEntity_RaceCar() {
            auto id = createEntity(entities::EntityType::RACE_CAR);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Texture>(id);
            createComponent<component::Shape>(id);
            createComponent<component::SoundTag>(id, component::SoundTag::ENGINE_IDLE);
            createComponent<component::TransformChildren>(id);
            createComponent<component::EntityAttachment>(id);

            createComponent<gameplay::CarLapInfo>(id, id);

            auto &carConfig = createComponent<component::CarConfig>(id);
            auto &car = createComponent<component::Car>(id);
            car.applyConfiguration(carConfig);

            auto &properties = createComponent<component::PhysicalProperties>(id);
            properties.physicalCategory = component::PhysicalCategory::RACE_CAR;
            properties.bodyType = component::BodyType::DYNAMIC;
            properties.highPrecision = true;

            assignTag<component::Tag_TextureShaded>(id);
            assignTag<component::Tag_Dynamic>(id);
            assignTag<component::Tag_Audible>(id);

            return id;
        }

        common::EntityID
        EntityFactory::createEntity_VehicleGun() {
            auto id = createEntity(entities::EntityType::VEHICLE_GUN);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Texture>(id);
            createComponent<component::Shape>(id);
            createComponent<component::EntityAttachee>(id);
            createComponent<component::TransformParent>(id);
            createComponent<component::GunCoolDown>(id);

            assignTag<component::Tag_Dynamic>(id);
            assignTag<component::Tag_TextureShaded>(id);

            return id;
        }

        common::EntityID
        EntityFactory::createEntity_Vehicle() {
            auto id = createEntity(entities::EntityType::VEHICLE);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Texture>(id);
            createComponent<component::Shape>(id);

            auto &properties = createComponent<component::PhysicalProperties>(id);
            properties.friction = 1.f;
            properties.density = 0.1f;
            properties.angularDamping = 2.f;
            properties.linearDamping = 2.f;
            properties.highPrecision = false;
            properties.bodyType = component::BodyType::DYNAMIC;
            properties.physicalCategory = component::PhysicalCategory::VEHICLE;


            assignTag<component::Tag_TextureShaded>(id);
            assignTag<component::Tag_Dynamic>(id);

            return id;
        }

        common::EntityID
        EntityFactory::createEntity_SimpleRocket() {
            auto id = createEntity(entities::EntityType::SIMPLE_ROCKET);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Texture>(id);
            createComponent<component::Shape>(id);
            createComponent<component::SoundTag>(id, component::SoundTag::ROCKET);
            createComponent<component::Trail>(id);

            auto &properties = createComponent<component::PhysicalProperties>(id);
            properties.friction = 1.f;
            properties.density = 0.1f;
            properties.angularDamping = 2.f;
            properties.linearDamping = 0.1f;
            properties.highPrecision = true;
            properties.bodyType = component::BodyType::DYNAMIC;
            properties.physicalCategory = component::PhysicalCategory::ROCKET;
            properties.collisionWithinCategory = false;

            auto &age = createComponent<component::Age>(id);
            age.maxAge = 5;

            assignTag<component::Tag_Dynamic>(id);
            assignTag<component::Tag_TextureShaded>(id);
            assignTag<component::Tag_Audible>(id);

            return id;
        }

        common::EntityID
        EntityFactory::createEntity_Wall() {
            auto id = createEntity(entities::EntityType::WALL);

            createComponent<component::Texture>(id);
            createComponent<component::Shape>(id);
            createComponent<component::Shape>(id);

            auto &properties = createComponent<component::PhysicalProperties>(id);
            properties.highPrecision = false;
            properties.bodyType = component::BodyType::STATIC;
            properties.physicalCategory = component::PhysicalCategory::WALL;

            assignTag<component::Tag_Static>(id);
            assignTag<component::Tag_TextureShaded>(id);

            return id;
        }

        common::EntityID
        EntityFactory::createEntity_VehicleTireFront() {
            auto id = createEntity(entities::EntityType::VEHICLE_TIRE_FRONT);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Texture>(id);
            createComponent<component::Shape>(id);

            createComponent<component::EntityAttachee>(id);
            createComponent<component::TransformParent>(id);

            assignTag<component::Tag_Dynamic>(id);
            assignTag<component::Tag_TextureShaded>(id);
            return id;
        }

        common::EntityID
        EntityFactory::createEntity_VehicleTireRear() {
            auto id = createEntity(entities::EntityType::VEHICLE_TIRE_REAR);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Texture>(id);
            createComponent<component::Shape>(id);

            createComponent<component::EntityAttachee>(id);
            createComponent<component::TransformParent>(id);

            assignTag<component::Tag_Dynamic>(id);
            assignTag<component::Tag_TextureShaded>(id);
            return id;
        }

        common::EntityID
        EntityFactory::createEntity_SimpleParticle() {
            auto id = createEntity(entities::EntityType::SIMPLE_PARTICLE);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Texture>(id);
            createComponent<component::Velocity>(id);
            createComponent<component::Age>(id);

            assignTag<component::Tag_Particle>(id);
            return id;
        }
    }
}