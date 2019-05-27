#include <oni-core/entities/oni-entities-manager.h>

#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <Box2D/Dynamics/b2World.h>

#include <oni-core/component/oni-component-physics.h>
#include <oni-core/component/oni-component-audio.h>
#include <oni-core/component/oni-component-gameplay.h>
#include <oni-core/level/oni-level-chunk.h>
#include <oni-core/gameplay/oni-gameplay-lap-tracker.h>
#include <oni-core/math/oni-math-rand.h>

namespace oni {
    namespace entities {
        EntityManager::EntityManager(entities::SimMode sMode,
                                     const math::ZLayerManager &zLayerManager,
                                     b2World &physicsWorld) : mSimMode(sMode), mZLayerManager(zLayerManager),
                                                              mPhysicsWorld(physicsWorld) {
            mRegistry = std::make_unique<entt::basic_registry<common::u32 >>();
            mLoader = std::make_unique<entt::basic_continuous_loader<common::EntityID>>(*mRegistry);
            mDispatcher = std::make_unique<entt::dispatcher>();
            mRand = std::make_unique<math::Rand>(0, 0);

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
        }

        EntityManager::~EntityManager() = default;

        void
        EntityManager::clearDeletedEntitiesList() {
            mDeletedEntities.clear();
        }

        void
        EntityManager::attach(common::EntityID parent,
                              common::EntityID child,
                              entities::EntityType parentType,
                              entities::EntityType childType) {
            auto &attachment = mRegistry->get<component::EntityAttachment>(parent);
            attachment.entities.emplace_back(child);
            attachment.entityTypes.emplace_back(childType);

            auto &attachee = mRegistry->get<component::EntityAttachee>(child);
            attachee.entityID = parent;
            attachee.entityType = parentType;
        }

        common::EntityID
        EntityManager::createComplementTo(common::EntityID id) {
            assert(mComplementaryEntities.find(id) == mComplementaryEntities.end());
            auto result = create();
            createComponent<entities::EntityType>(result, entities::EntityType::COMPLEMENT);
            mComplementaryEntities[id] = result;
            return result;
        }

        common::EntityID
        EntityManager::getComplementOf(common::EntityID id) {
            assert(mComplementaryEntities.find(id) != mComplementaryEntities.end());
            return mComplementaryEntities[id];
        }

        bool
        EntityManager::hasComplement(common::EntityID id) {
            return mComplementaryEntities.find(id) != mComplementaryEntities.end();
        }

        size_t
        EntityManager::size() noexcept {
            auto result = mRegistry->size();
            return result;
        }

        common::EntityID
        EntityManager::map(common::EntityID entityID) {
            auto result = mLoader->map(entityID);
            if (result == entt::null) {
                return 0;
            }
            return result;
        }

        const std::vector<entities::DeletedEntity> &
        EntityManager::getDeletedEntities() const {
            return mDeletedEntities;
        }

        void
        EntityManager::markForNetSync(common::EntityID entity) {
            accommodate<component::Tag_NetworkSyncComponent>(entity);
        }

        void
        EntityManager::dispatchEvents() {
            mDispatcher->update();
        }

        common::EntityID
        EntityManager::create() {
            auto result = mRegistry->create();
            return result;
        }

        void
        EntityManager::destroy(common::EntityID entityID) {
            mRegistry->destroy(entityID);
        }

        void
        EntityManager::destroyAndTrack(common::EntityID id) {
            auto type = mRegistry->get<entities::EntityType>(id);
            mDeletedEntities.push_back({id, type});
            mRegistry->destroy(id);
        }

        bool
        EntityManager::valid(common::EntityID entityID) {
            return mRegistry->valid(entityID);
        }

        void
        EntityManager::markForDeletion(common::EntityID id) {
            mEntitiesToDelete.push_back(id);
            mEntitiesToDeletePolicy.push_back(mEntityOperationPolicy);
        }

        void
        EntityManager::markForDeletion(common::EntityID id,
                                       const entities::EntityOperationPolicy &policy) {
            mEntitiesToDelete.push_back(id);
            mEntitiesToDeletePolicy.push_back(policy);
        }

        void
        EntityManager::flushDeletions() {
            for (common::size i = 0; i < mEntitiesToDelete.size(); ++i) {
                removeEntity(mEntitiesToDelete[i], mEntitiesToDeletePolicy[i]);
            }

            mEntitiesToDelete.clear();
            mEntitiesToDeletePolicy.clear();
        }

        common::EntityID
        EntityManager::createEntity(entities::EntityType type) {
            auto id = mRegistry->create();
            assignSimMode(id, mSimMode);
            if (mEntityOperationPolicy.track) {
                assert(mSimMode == entities::SimMode::SERVER);
                assignTag<component::Tag_NetworkSyncEntity>(id);
            }
            createComponent<entities::EntityType>(id, type);
            return id;
        }

        void
        EntityManager::removeEntity(common::EntityID id) {
            removeEntity(id, mEntityOperationPolicy);
        }

        void
        EntityManager::removeEntity(common::EntityID id,
                                    const entities::EntityOperationPolicy &policy) {
            if (policy.safe && !valid(id)) {
                return;
            }

            if (mRegistry->has<component::EntityAttachment>(id)) {
                for (auto &&childID: mRegistry->get<component::EntityAttachment>(id).entities) {
                    removeEntity(childID, policy);
                }
            }

            if (mRegistry->has<component::PhysicalProperties>(id)) {
                removePhysicalBody(id);
            }

            if (mRegistry->has<component::Texture>(id)) {
                // TODO: Clean up the texture in video-memory or tag it stale or something if I end up having a asset manager
            }

            if (mRegistry->has<component::SoundTag>(id)) {
                // TODO: Same as texture, audio system needs to free the resource or let its resource manager at least know about this
            }

            if (policy.track) {
                destroyAndTrack(id);
            } else {
                destroy(id);
            }
        }

        void
        EntityManager::createPhysics(common::EntityID id,
                                     const component::WorldP3D &pos,
                                     const math::vec2 &size,
                                     common::r32 heading) {
            auto &props = mRegistry->get<component::PhysicalProperties>(id);
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
            mEntityBodyMap[id] = body;
        }

        void
        EntityManager::removePhysicalBody(common::EntityID id) {
            auto *body = mEntityBodyMap[id];
            assert(body);
            mPhysicsWorld.DestroyBody(body);
        }

        void
        EntityManager::assignSimMode(common::EntityID id,
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
        EntityManager::setTexture(common::EntityID id,
                                  std::string_view path) {
            mRegistry->get<component::Texture>(id).path = path;
        }

        void
        EntityManager::setRandAge(common::EntityID id,
                                  common::r32 lower,
                                  common::r32 upper) {
            auto &age = mRegistry->get<component::Age>(id);
            age.maxAge = mRand->next_r32(lower, upper);
        }

        void
        EntityManager::setRandHeading(common::EntityID id) {
            auto &heading = mRegistry->get<component::Heading>(id);
            heading.value = mRand->next_r32(0.f, common::FULL_CIRCLE_IN_RAD);
        }

        void
        EntityManager::setRandVelocity(common::EntityID id,
                                       common::u32 lower,
                                       common::u32 upper) {
            auto &velocity = mRegistry->get<component::Velocity>(id);
            velocity.currentVelocity = mRand->next(lower, upper);
            velocity.maxVelocity = velocity.currentVelocity;
        }

        void
        EntityManager::setWorldP3D(common::EntityID id,
                                   common::r32 x,
                                   common::r32 y,
                                   common::r32 z) {
            auto &pos = mRegistry->get<component::WorldP3D>(id);
            pos.x = x;
            pos.y = y;
            pos.z = z;
        }

        void
        EntityManager::setScale(common::EntityID id,
                                common::r32 x,
                                common::r32 y) {
            auto &scale = mRegistry->get<component::Scale>(id);
            scale.x = x;
            scale.y = y;
        }

        void
        EntityManager::setHeading(common::EntityID id,
                                  common::r32 heading) {
            auto &h = mRegistry->get<component::Heading>(id);
            h.value = heading;
        }

        void
        EntityManager::setText(common::EntityID id,
                               std::string_view content) {
            auto &text = mRegistry->get<component::Text>(id);
            text.textContent = content;

        }

        void
        EntityManager::setApperance(common::EntityID id,
                                    common::r32 red,
                                    common::r32 green,
                                    common::r32 blue,
                                    common::r32 alpha) {
            auto &apperance = mRegistry->get<component::Appearance>(id);
            apperance.color.x = red;
            apperance.color.y = green;
            apperance.color.z = blue;
            apperance.color.w = alpha;
        }

        b2Body *
        EntityManager::getEntityBody(common::EntityID id) {
            auto result = mEntityBodyMap[id];
            assert(result);
            return result;
        }

        common::EntityID
        EntityManager::createEntity_SmokeCloud() {
            auto id = createEntity(entities::EntityType::SMOKE);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Texture>(id);
            createComponent<component::Age>(id);
            createComponent<component::Velocity>(id);

            assignTag<component::Tag_TextureShaded>(id);
            assignTag<component::Tag_Dynamic>(id);

            return id;
        }

        common::EntityID
        EntityManager::createEntity_RaceCar() {
            auto id = createEntity(entities::EntityType::RACE_CAR);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Texture>(id);
            createComponent<component::SoundTag>(id, component::SoundTag::ENGINE_IDLE);
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
        EntityManager::createEntity_VehicleGun() {
            auto id = createEntity(entities::EntityType::VEHICLE_GUN);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Texture>(id);
            createComponent<component::EntityAttachee>(id);
            createComponent<component::GunCoolDown>(id);

            assignTag<component::Tag_Dynamic>(id);
            assignTag<component::Tag_TextureShaded>(id);

            return id;
        }

        common::EntityID
        EntityManager::createEntity_Vehicle() {
            auto id = createEntity(entities::EntityType::VEHICLE);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Texture>(id);

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
        EntityManager::createEntity_SimpleRocket() {
            auto id = createEntity(entities::EntityType::SIMPLE_ROCKET);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Texture>(id);
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
        EntityManager::createEntity_Wall() {
            auto id = createEntity(entities::EntityType::WALL);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Texture>(id);

            auto &properties = createComponent<component::PhysicalProperties>(id);
            properties.highPrecision = false;
            properties.bodyType = component::BodyType::STATIC;
            properties.physicalCategory = component::PhysicalCategory::WALL;

            assignTag<component::Tag_Static>(id);
            assignTag<component::Tag_TextureShaded>(id);

            return id;
        }

        common::EntityID
        EntityManager::createEntity_VehicleTireFront() {
            auto id = createEntity(entities::EntityType::VEHICLE_TIRE_FRONT);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Texture>(id);

            createComponent<component::EntityAttachee>(id);

            assignTag<component::Tag_Dynamic>(id);
            assignTag<component::Tag_TextureShaded>(id);
            return id;
        }

        common::EntityID
        EntityManager::createEntity_VehicleTireRear() {
            auto id = createEntity(entities::EntityType::VEHICLE_TIRE_REAR);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Texture>(id);

            createComponent<component::EntityAttachee>(id);

            assignTag<component::Tag_Dynamic>(id);
            assignTag<component::Tag_TextureShaded>(id);
            return id;
        }

        common::EntityID
        EntityManager::createEntity_SimpleParticle() {
            auto id = createEntity(entities::EntityType::SIMPLE_PARTICLE);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Texture>(id);
            createComponent<component::Velocity>(id);
            createComponent<component::Age>(id);

            assignTag<component::Tag_TextureShaded>(id);
            return id;
        }

        common::EntityID
        EntityManager::createEntity_SimpleBlastParticle() {
            auto id = createEntity(entities::EntityType::SIMPLE_BLAST_PARTICLE);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Texture>(id);
            createComponent<component::Velocity>(id);
            createComponent<component::Age>(id);

            assignTag<component::Tag_TextureShaded>(id);
            assignTag<component::Tag_SplatOnDeath>(id);

            return id;
        }

        common::EntityID
        EntityManager::createEntity_Text() {
            auto id = createEntity(entities::EntityType::TEXT);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Text>(id);

            assignTag<component::Tag_Static>(id);
            return id;
        }

        common::EntityID
        EntityManager::createEntity_WorldChunk() {
            auto id = createEntity(entities::EntityType::WORLD_CHUNK);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Texture>(id);
            createComponent<level::Chunk>(id);

            assignTag<component::Tag_Static>(id);
            assignTag<component::Tag_TextureShaded>(id);

            return id;
        }

        common::EntityID
        EntityManager::createEntity_DebugWorldChunk() {
            auto id = createEntity(entities::EntityType::DEBUG_WORLD_CHUNK);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Appearance>(id);
            createComponent<level::Chunk>(id);

            assignTag<component::Tag_Static>(id);
            assignTag<component::Tag_ColorShaded>(id);

            return id;
        }

        common::EntityID
        EntityManager::createEntity_SimpleSpriteColored() {
            auto id = createEntity(entities::EntityType::SIMPLE_SPRITE);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Appearance>(id);

            assignTag<component::Tag_Static>(id);
            assignTag<component::Tag_ColorShaded>(id);

            return id;
        }

        common::EntityID
        EntityManager::createEntity_SimpleSpriteTextured() {
            auto id = createEntity(entities::EntityType::SIMPLE_SPRITE);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Texture>(id);

            assignTag<component::Tag_Static>(id);
            assignTag<component::Tag_TextureShaded>(id);

            return id;
        }
    }
}