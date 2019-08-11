#include <oni-core/entities/oni-entities-manager.h>

#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <Box2D/Dynamics/b2World.h>

#include <oni-core/component/oni-component-physics.h>
#include <oni-core/component/oni-component-audio.h>
#include <oni-core/component/oni-component-gameplay.h>
#include <oni-core/component/oni-component-type.h>
#include <oni-core/gameplay/oni-gameplay-lap-tracker.h>
#include <oni-core/graphic/oni-graphic-texture-manager.h>
#include <oni-core/level/oni-level-chunk.h>
#include <oni-core/math/oni-math-rand.h>

namespace oni {
    namespace entities {
        EntityManager::EntityManager(entities::SimMode sMode,
                                     b2World *physicsWorld) : mSimMode(sMode),
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
                case SimMode::CLIENT_SIDE_SERVER: {
                    mEntityOperationPolicy = entities::EntityOperationPolicy::clientServer();
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

        size_t
        EntityManager::size() {
            auto result = mRegistry->size();
            return result;
        }

        common::size
        EntityManager::alive() {
            auto result = mRegistry->alive();
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
            if (mEntityOperationPolicy.track) {
                assert(mSimMode == entities::SimMode::SERVER);
                accommodate<component::Tag_NetworkSyncComponent>(entity);
            }
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
        }

        void
        EntityManager::flushDeletions() {
            for (auto &&i : mEntitiesToDelete) {
                deleteEntity(i);
            }

            mEntitiesToDelete.clear();
        }

        common::EntityID
        EntityManager::createEntity(entities::EntityType type) {
            assert(mSimMode == entities::SimMode::SERVER || mSimMode == entities::SimMode::CLIENT);
            auto id = mRegistry->create();
            if (mEntityOperationPolicy.track) {
                assert(mSimMode == entities::SimMode::SERVER);
                assignTag<component::Tag_NetworkSyncEntity>(id);
            }
            createComponent<entities::EntityType>(id, type);
            return id;
        }

        void
        EntityManager::deleteEntity(common::EntityID id) {
            deleteEntity(id, mEntityOperationPolicy);
        }

        void
        EntityManager::deleteEntity(common::EntityID id,
                                    const entities::EntityOperationPolicy &policy) {
            if (policy.safe && !valid(id)) {
                return;
            }

            if (mRegistry->has<component::EntityAttachment>(id)) {
                for (auto &&childID: mRegistry->get<component::EntityAttachment>(id).entities) {
                    deleteEntity(childID, policy);
                }
            }

            if (mRegistry->has<component::PhysicalProperties>(id)) {
                removePhysicalBody(id);
            }

            if (mRegistry->has<component::Texture>(id)) {
                // TODO: Clean up the texture in video-memory or tag it stale or something if I end up having a asset manager
            }

            if (mRegistry->has<component::Sound_Tag>(id)) {
                // TODO: Same as texture, audio system needs to free the resource or let its resource manager at least know about this
            }

            if (policy.track) {
                destroyAndTrack(id);
            } else {
                destroy(id);
            }
        }

        void
        EntityManager::printEntityType(common::EntityID id) {
            const auto &t = mRegistry->get<entities::EntityType>(id);
            auto name = std::string();
            switch (t) {
                case entities::EntityType::BACKGROUND: {
                    name = "background";
                    break;
                }
                case entities::EntityType::ROAD: {
                    name = "road";
                    break;
                }
                case entities::EntityType::WALL: {
                    name = "wall";
                    break;
                }
                case entities::EntityType::RACE_CAR: {
                    name = "race_car";
                    break;
                }
                case entities::EntityType::VEHICLE: {
                    name = "vehicle";
                    break;
                }
                case entities::EntityType::VEHICLE_GUN: {
                    name = "vehicle_gun";
                    break;
                }
                case entities::EntityType::VEHICLE_TIRE_REAR: {
                    name = "vehicle_tire_rear";
                    break;
                }
                case entities::EntityType::VEHICLE_TIRE_FRONT: {
                    name = "vehicle_tire_front";
                    break;
                }
                case entities::EntityType::UI: {
                    name = "ui";
                    break;
                }
                case entities::EntityType::CANVAS: {
                    name = "canvas";
                    break;
                }
                case entities::EntityType::SIMPLE_SPRITE: {
                    name = "simple_sprite";
                    break;
                }
                case entities::EntityType::SIMPLE_PARTICLE: {
                    name = "simple_particle";
                    break;
                }
                case entities::EntityType::SIMPLE_BLAST_PARTICLE: {
                    name = "simple_blast_particle";
                    break;
                }
                case entities::EntityType::SIMPLE_ROCKET: {
                    name = "simple_rocket";
                    break;
                }
                case entities::EntityType::TRAIL_PARTICLE: {
                    name = "trail_particle";
                    break;
                }
                case entities::EntityType::TEXT: {
                    name = "text";
                    break;
                }
                case entities::EntityType::WORLD_CHUNK: {
                    name = "world_chunk";
                    break;
                }
                case entities::EntityType::DEBUG_WORLD_CHUNK: {
                    name = "debug_world_chunk";
                    break;
                }
                case entities::EntityType::SMOKE_CLOUD: {
                    name = "smoke_cloud";
                    break;
                }
                case entities::EntityType::COMPLEMENT: {
                    name = "complement";
                    break;
                }
                case entities::EntityType::UNKNOWN:
                case entities::EntityType::LAST:
                default: {
                    assert(false);
                    break;
                }
            }
            std::cout << name << '\n';
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
            bodyDef.gravityScale = props.gravityScale;

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
                    body = mPhysicsWorld->CreateBody(&bodyDef);

                    b2FixtureDef collisionSensor;
                    collisionSensor.isSensor = true;
                    collisionSensor.shape = &shape;
                    collisionSensor.density = props.density;
                    collisionSensor.friction = props.friction;

                    if (!props.collisionWithinCategory) {
                        collisionSensor.filter.groupIndex = -static_cast<common::i16>(props.physicalCategory);
                    }

                    if (props.disableCollision) {
                        fixtureDef.isSensor = true;
                    } else {
                        body->CreateFixture(&collisionSensor);
                    }
                    body->CreateFixture(&fixtureDef);
                    break;
                }
                case component::BodyType::STATIC: {
                    bodyDef.position.x = pos.x;
                    bodyDef.position.y = pos.y;
                    bodyDef.type = b2_staticBody;
                    body = mPhysicsWorld->CreateBody(&bodyDef);
                    body->CreateFixture(&shape, 0.f);
                    break;
                }
                case component::BodyType::KINEMATIC: {
                    bodyDef.position.x = pos.x;
                    bodyDef.position.y = pos.y;
                    bodyDef.type = b2_kinematicBody;
                    body = mPhysicsWorld->CreateBody(&bodyDef);
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
            mPhysicsWorld->DestroyBody(body);
        }

        void
        EntityManager::setRandAge(common::EntityID id,
                                  common::r32 lower,
                                  common::r32 upper) {
            auto &age = mRegistry->get<component::Age>(id);
            age.maxAge = mRand->next_r32(lower, upper);
        }

        void
        EntityManager::setRandVelocity(common::EntityID id,
                                       common::u32 lower,
                                       common::u32 upper) {
            auto &velocity = mRegistry->get<component::Velocity>(id);
            velocity.current = mRand->next(lower, upper);
            velocity.max = velocity.current;
        }

        common::r32
        EntityManager::setRandHeading(common::EntityID id) {
            return setRandHeading(id, 0.f, common::FULL_CIRCLE_IN_RAD);
        }

        common::r32
        EntityManager::setRandHeading(common::EntityID id,
                                      common::r32 lower,
                                      common::r32 upper) {
            auto &heading = mRegistry->get<component::Heading>(id);
            heading.value = mRand->next_r32(lower, upper);
            return heading.value;
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
        EntityManager::setEntityPreset(common::EntityID id,
                                       component::EntityPreset tag) {

            auto &ep = mRegistry->get<component::EntityPreset>(id);
            ep = tag;
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
        EntityManager::setColor(common::EntityID id,
                                common::r32 red,
                                common::r32 green,
                                common::r32 blue,
                                common::r32 alpha) {
            auto &color = mRegistry->get<component::Color>(id);
            color.set_r(red);
            color.set_g(green);
            color.set_b(blue);
            color.set_a(alpha);
        }

        b2Body *
        EntityManager::getEntityBody(common::EntityID id) {
            auto result = mEntityBodyMap[id];
            assert(result);
            return result;
        }

        entities::SimMode
        EntityManager::getSimMode() {
            return mSimMode;
        }

        common::EntityID
        EntityManager::createEntity_SmokeCloud() {
            assert(mSimMode == entities::SimMode::CLIENT);
            auto id = createEntity(entities::EntityType::SMOKE_CLOUD);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Age>(id);
            createComponent<component::Velocity>(id);
            createComponent<component::EntityPreset>(id, component::EntityPreset::SMOKE_WHITE);
            // TODO: needs physics

            assignTag<component::Tag_Dynamic>(id);

            return id;
        }

        common::EntityID
        EntityManager::createEntity_RaceCar() {
            assert(mSimMode == entities::SimMode::SERVER);

            auto id = createEntity(entities::EntityType::RACE_CAR);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Sound>(id, component::Sound_Tag::ENGINE_IDLE, component::ChannelGroup::EFFECT);
            createComponent<component::EntityAttachment>(id);
            createComponent<component::EntityPreset>(id, component::EntityPreset::RACE_CAR_DEFAULT);

            auto &ph = createComponent<component::WorldP3D_History>(id);
            ph.size = 20;

            createComponent<gameplay::CarLapInfo>(id, id);

            auto &carConfig = createComponent<component::CarConfig>(id);
            auto &car = createComponent<component::Car>(id);
            car.applyConfiguration(carConfig);

            auto &properties = createComponent<component::PhysicalProperties>(id);
            properties.physicalCategory = component::PhysicalCategory::RACE_CAR;
            properties.bodyType = component::BodyType::DYNAMIC;
            properties.highPrecision = true;

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
            createComponent<component::EntityAttachee>(id);
            createComponent<component::GunCoolDown>(id);
            createComponent<component::EntityPreset>(id, component::EntityPreset::VEHICLE_GUN_DEFAULT);

            assignTag<component::Tag_Dynamic>(id);

            return id;
        }

        common::EntityID
        EntityManager::createEntity_Vehicle() {
            auto id = createEntity(entities::EntityType::VEHICLE);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);
            createComponent<component::EntityPreset>(id, component::EntityPreset::VEHICLE_DEFAULT);

            auto &properties = createComponent<component::PhysicalProperties>(id);
            properties.friction = 1.f;
            properties.density = 0.1f;
            properties.angularDamping = 2.f;
            properties.linearDamping = 2.f;
            properties.highPrecision = false;
            properties.bodyType = component::BodyType::DYNAMIC;
            properties.physicalCategory = component::PhysicalCategory::VEHICLE;

            assignTag<component::Tag_Dynamic>(id);

            return id;
        }

        common::EntityID
        EntityManager::createEntity_SimpleRocket() {
            auto id = createEntity(entities::EntityType::SIMPLE_ROCKET);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Sound>(id, component::Sound_Tag::ROCKET_BURN, component::ChannelGroup::EFFECT);
            createComponent<component::EntityPreset>(id, component::EntityPreset::ROCKET_DEFAULT);
            auto &pt = createComponent<component::ParticleEmitter>(id);
            pt.size = 3.f;
            pt.tag = component::EntityPreset::CLOUD_BLACK;
            auto &ph = createComponent<component::WorldP3D_History>(id);
            ph.size = 10;

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

            auto &jet = createComponent<component::JetForce>(id);
            jet.fuze = 3.f;
            jet.force = 3.f;

            assignTag<component::Tag_Dynamic>(id);
            assignTag<component::Tag_Audible>(id);

            return id;
        }

        common::EntityID
        EntityManager::createEntity_Wall() {
            auto id = createEntity(entities::EntityType::WALL);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);

            auto &properties = createComponent<component::PhysicalProperties>(id);
            properties.highPrecision = false;
            properties.bodyType = component::BodyType::STATIC;
            properties.physicalCategory = component::PhysicalCategory::WALL;
            createComponent<component::EntityPreset>(id, component::EntityPreset::WALL_VERTICAL);

            assignTag<component::Tag_Static>(id);

            return id;
        }

        common::EntityID
        EntityManager::createEntity_VehicleTireFront() {
            auto id = createEntity(entities::EntityType::VEHICLE_TIRE_FRONT);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);
            createComponent<component::EntityAttachee>(id);
            createComponent<component::EntityPreset>(id, component::EntityPreset::RACE_CAR_TIRE_DEFAULT);

            assignTag<component::Tag_Dynamic>(id);
            return id;
        }

        common::EntityID
        EntityManager::createEntity_VehicleTireRear() {
            auto id = createEntity(entities::EntityType::VEHICLE_TIRE_REAR);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);
            createComponent<component::EntityAttachee>(id);
            createComponent<component::EntityPreset>(id, component::EntityPreset::RACE_CAR_TIRE_WITH_TRAIL);

            assignTag<component::Tag_Dynamic>(id);
            return id;
        }

        common::EntityID
        EntityManager::createEntity_SimpleParticle() {
            assert(mSimMode == entities::SimMode::CLIENT);
            auto id = createEntity(entities::EntityType::SIMPLE_PARTICLE);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Age>(id);
            createComponent<component::Velocity>(id);
            createComponent<component::EntityPreset>(id, component::EntityPreset::CLOUD_WHITE);

            return id;
        }

        common::EntityID
        EntityManager::createEntity_SimpleBlastParticle() {
            assert(mSimMode == entities::SimMode::CLIENT);
            auto id = createEntity(entities::EntityType::SIMPLE_BLAST_PARTICLE);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Age>(id);
            createComponent<component::EntityPreset>(id, component::EntityPreset::BLAST_PARTICLE_DEFAULT);
            auto &scale = createComponent<component::Scale>(id);
            scale.x = mRand->next_r32(0.1f, 0.2f);
            scale.y = scale.x;

            auto &props = createComponent<component::PhysicalProperties>(id);
            props.friction = 2.f;
            props.density = 0.5f;
            props.angularDamping = 2.f;
            props.linearDamping = 2 + (scale.x + scale.y) / 2;
            props.highPrecision = false;
            props.disableCollision = true;
            props.bodyType = component::BodyType::DYNAMIC;
            props.physicalCategory = component::PhysicalCategory::PROJECTILE;

            //assignTag<component::Tag_SplatOnDeath>(id);
            assignTag<component::Tag_Dynamic>(id);

            return id;
        }

        common::EntityID
        EntityManager::createEntity_SimpleBlastAnimation() {
            assert(mSimMode == entities::SimMode::CLIENT);
            auto id = createEntity(entities::EntityType::SIMPLE_BLAST_ANIMATION);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);
            createComponent<component::EntityPreset>(id, component::EntityPreset::BLAST_ANIMATION_DEFAULT);

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
            assert(mSimMode == entities::SimMode::SERVER);
            auto id = createEntity(entities::EntityType::WORLD_CHUNK);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);
            createComponent<component::EntityPreset>(id, component::EntityPreset::BACKGROUND_DEFAULT);
            createComponent<level::Chunk>(id);

            assignTag<component::Tag_Static>(id);

            return id;
        }

        common::EntityID
        EntityManager::createEntity_DebugWorldChunk() {
            assert(mSimMode == entities::SimMode::SERVER);
            auto id = createEntity(entities::EntityType::DEBUG_WORLD_CHUNK);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Heading>(id);
            createComponent<component::Scale>(id);
            createComponent<component::EntityPreset>(id, component::EntityPreset::BACKGROUND_DEBUG);

            assignTag<component::Tag_Static>(id);

            return id;
        }

        common::EntityID
        EntityManager::createEntity_SimpleSpriteColored() {
            // TODO: Implement
            assert(false);
            auto id = createEntity(entities::EntityType::SIMPLE_SPRITE);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Heading>(id);

            assignTag<component::Tag_Static>(id);

            return id;
        }

        common::EntityID
        EntityManager::createEntity_SimpleSpriteTextured() {
            // TODO: Implement
            assert(false);
            auto id = createEntity(entities::EntityType::SIMPLE_SPRITE);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Heading>(id);

            assignTag<component::Tag_Static>(id);

            return id;
        }

        common::EntityID
        EntityManager::createEntity_CanvasTile() {
            assert(mSimMode == entities::SimMode::CLIENT);
            auto id = createEntity(entities::EntityType::CANVAS);

            createComponent<component::WorldP3D>(id);
            createComponent<component::Scale>(id);
            createComponent<component::Heading>(id);
            createComponent<component::EntityPreset>(id, component::EntityPreset::CANVAS);

            assignTag<component::Tag_Static>(id);

            return id;
        }
    }
}