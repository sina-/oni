#include <oni-core/physics/oni-physics-dynamics.h>

#include <Box2D/Box2D.h>
#include <GLFW/glfw3.h>

#include <oni-core/entities/oni-entities-manager.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-physic.h>
#include <oni-core/component/oni-component-visual.h>
#include <oni-core/common/oni-common-const.h>
#include <oni-core/math/oni-math-rand.h>
#include <oni-core/graphic/oni-graphic-window.h>
#include <oni-core/physics/oni-physics-car.h>
#include <oni-core/math/oni-math-transformation.h>
#include <oni-core/physics/oni-physics-projectile.h>
#include <oni-core/entities/oni-entities-factory.h>
#include <oni-core/component/oni-component-hierarchy.h>
#include <oni-core/game/oni-game-event.h>

namespace oni {
    namespace physics {
        class CollisionHandler : public b2ContactListener {
        public:
            void
            BeginContact(b2Contact *contact) override {
                auto *fixtureA = contact->GetFixtureA();
                auto *fixtureB = contact->GetFixtureB();

                //if (fixtureA->IsSensor()) {
                void *userData = fixtureA->GetBody()->GetUserData();
                if (userData) {
                    bool *colliding = static_cast<bool *> (userData);
                    *colliding = true;
                }
                //}

                //if (fixtureB->IsSensor()) {
                userData = fixtureB->GetBody()->GetUserData();
                if (userData) {
                    bool *colliding = static_cast<bool *> (userData);
                    *colliding = true;
                }
                //}
            }

            void
            EndContact(b2Contact *contact) override {
                auto *fixtureA = contact->GetFixtureA();
                auto *fixtureB = contact->GetFixtureB();

                //if (fixtureA->IsSensor()) {
                void *userData = fixtureA->GetBody()->GetUserData();
                if (userData) {
                    bool *colliding = static_cast<bool *> (userData);
                    *colliding = false;
                }
                //}

                //if (fixtureB->IsSensor()) {
                userData = fixtureB->GetBody()->GetUserData();
                if (userData) {
                    bool *colliding = static_cast<bool *> (userData);
                    *colliding = false;
                }
                //}
            }

/*            void PreSolve(b2Contact *contact, const b2Manifold *oldManifold) override {
                std::cout << "PreSolve\n";
            }

            void PostSolve(b2Contact *contact, const b2ContactImpulse *impulse) override {
                std::cout << "PostSolve\n";
            }*/
        };

        Dynamics::Dynamics(common::real32 tickFreq)
                : mTickFrequency(tickFreq) {
            b2Vec2 gravity(0.0f, 0.0f);
            mCollisionHandler = std::make_unique<CollisionHandler>();
            mPhysicsWorld = std::make_unique<b2World>(gravity);
            mProjectile = std::make_unique<Projectile>(mPhysicsWorld.get());
            mRand = std::make_unique<math::Rand>(0);

            mCollisionHandlers[component::PhysicalCategory::ROCKET] =
                    std::bind(&Dynamics::handleRocketCollision, this,
                              std::placeholders::_1,
                              std::placeholders::_2,
                              std::placeholders::_3,
                              std::placeholders::_4);

            mCollisionHandlers[component::PhysicalCategory::VEHICLE] =
                    std::bind(&Dynamics::handleVehicleCollision, this,
                              std::placeholders::_1,
                              std::placeholders::_2,
                              std::placeholders::_3,
                              std::placeholders::_4);

            mCollisionHandlers[component::PhysicalCategory::RACE_CAR] =
                    std::bind(&Dynamics::handleRaceCarCollision, this,
                              std::placeholders::_1,
                              std::placeholders::_2,
                              std::placeholders::_3,
                              std::placeholders::_4);

            mCollisionHandlers[component::PhysicalCategory::WALL] =
                    std::bind(&Dynamics::handleCollision, this,
                              std::placeholders::_1,
                              std::placeholders::_2,
                              std::placeholders::_3,
                              std::placeholders::_4);

            mCollisionHandlers[component::PhysicalCategory::GENERIC] =
                    std::bind(&Dynamics::handleCollision, this,
                              std::placeholders::_1,
                              std::placeholders::_2,
                              std::placeholders::_3,
                              std::placeholders::_4);

            // TODO: Can't get this working, its unreliable, when there are lot of collisions in the world, it keeps
            // skipping some of them!
            // mPhysicsWorld->SetContactListener(mCollisionHandler.get());
        }

        Dynamics::~Dynamics() = default;

        void
        Dynamics::tickServerSide(entities::EntityFactory &entityFactory,
                                 entities::ClientDataManager &clientData,
                                 common::real64 tickTime) {

            auto &manager = entityFactory.getEntityManager();
            std::map<common::EntityID, io::CarInput> carInput{};
            std::vector<common::EntityID> entitiesToBeUpdated{};

            // Apply user input
            {
                auto carView = manager.createView<component::WorldP3D, component::Heading, component::Scale, component::Car, component::CarConfig>();
                for (auto &&entity: carView) {
                    auto input = clientData.getClientInput(entity);
                    // NOTE: This could happen just at the moment when a client joins, an entity is created by the
                    // client data structures are not initialized.
                    if (!input) {
                        continue;
                    }

                    auto &car = carView.get<component::Car>(entity);
                    const auto &carConfig = carView.get<component::CarConfig>(entity);

                    if (input->isPressed(GLFW_KEY_W) || input->isPressed(GLFW_KEY_UP)) {
                        // TODO: When using game-pad, this value will vary between (0.0f...1.0f)
                        carInput[entity].throttle = 1.0f;
                    }
                    if (input->isPressed(GLFW_KEY_A) || input->isPressed(GLFW_KEY_LEFT)) {
                        carInput[entity].left = 1.0f;
                    }
                    if (input->isPressed(GLFW_KEY_S) || input->isPressed(GLFW_KEY_DOWN)) {
                        carInput[entity].throttle = -1.0f;
                    }
                    if (input->isPressed(GLFW_KEY_D) || input->isPressed(GLFW_KEY_RIGHT)) {
                        carInput[entity].right = 1.0f;
                    }
                    if (input->isPressed(GLFW_KEY_LEFT_SHIFT)) {
                        car.velocity = car.velocity + math::vec2{static_cast<common::real32>(cos(car.heading)),
                                                                 static_cast<common::real32>(sin(car.heading))};
                    }
                    if (input->isPressed(GLFW_KEY_SPACE)) {
                        if (car.accumulatedEBrake < 1.0f) {
                            car.accumulatedEBrake += 0.01f;
                        }
                        carInput[entity].eBrake = static_cast<common::real32>(car.accumulatedEBrake);
                    } else {
                        car.accumulatedEBrake = 0.0f;
                    }

                    auto steerInput = carInput[entity].left - carInput[entity].right;
                    if (car.smoothSteer) {
                        car.steer = applySmoothSteer(car, steerInput, tickTime);
                    } else {
                        car.steer = steerInput;
                    }


                    if (car.safeSteer) {
                        car.steer = applySafeSteer(car, steerInput);
                    }

                    car.steerAngle = car.steer * carConfig.maxSteer;

                    tickCar(car, carConfig, carInput[entity], tickTime);

                    auto &carPos = carView.get<component::WorldP3D>(entity);
                    auto &heading = carView.get<component::Heading>(entity);
                    auto &scale = carView.get<component::Scale>(entity);

                    auto velocity = car.velocityLocal.len();
                    auto distanceFromCamera = 1 + velocity * 2 / car.maxVelocityAbsolute;
                    if (std::abs(carPos.x - car.position.x) > common::EP ||
                        std::abs(carPos.y - car.position.y) > common::EP ||
                        std::abs(carPos.z - car.heading) > common::EP ||
                        std::abs(car.distanceFromCamera - distanceFromCamera) > common::EP
                            ) {
                        carPos.x = car.position.x;
                        carPos.y = car.position.y;
                        heading.value = static_cast<common::real32>(car.heading);
                        car.distanceFromCamera = distanceFromCamera;

                        updateTransforms(manager, entity, carPos, heading, scale);
                        entitiesToBeUpdated.push_back(entity);
                    }
                }
            }

            // Update box2d world
            {
                // TODO: entity registry has pointers to mPhysicsWorld internal data structures :(
                // One way to hide it is to provide a function in physics library that creates physical entities
                // for a given entity id an maintains an internal mapping between them without leaking the
                // implementation to outside.
                mPhysicsWorld->Step(mTickFrequency, 6, 2);
            }


            // Handle car collisions
            {
                auto carPhysicsView = manager.createView<component::Car, component::PhysicalProperties>();
                for (auto entity: carPhysicsView) {
                    auto &props = carPhysicsView.get<component::PhysicalProperties>(entity);
                    auto &car = carPhysicsView.get<component::Car>(entity);
                    // NOTE: If the car was in collision previous tick, that is what isColliding is tracking,
                    // just apply user input to box2d representation of physical body without syncing
                    // car dynamics with box2d physics, that way the next tick if the
                    // car was heading out of collision it will start sliding out and things will run smoothly according
                    // to car dynamics calculation. If the car is still heading against other objects, it will be
                    // stuck as it was and I will skip dynamics and just sync it to  position and orientation
                    // from box2d. This greatly improves game feeling when there are collisions and
                    // solves lot of stickiness issues.
                    if (car.isColliding) {
                        // TODO: Right now 30 is just an arbitrary multiplier, maybe it should be based on some value in
                        // carconfig?
                        // TODO: Test other type of forces if there is a combination of acceleration and steering to sides
                        props.body->ApplyForceToCenter(
                                b2Vec2(static_cast<common::real32>(std::cos(car.heading) * 30 *
                                                                   carInput[entity].throttle),
                                       static_cast<common::real32>(std::sin(car.heading) * 30 *
                                                                   carInput[entity].throttle)),
                                true);
                        car.isColliding = false;
                    } else {
                        if (isColliding(props.body)) {
                            car.velocity = math::vec2{props.body->GetLinearVelocity().x,
                                                      props.body->GetLinearVelocity().y};
                            car.angularVelocity = props.body->GetAngularVelocity();
                            car.position = math::vec2{props.body->GetPosition().x, props.body->GetPosition().y};
                            car.heading = props.body->GetAngle();
                            car.isColliding = true;
                        } else {
                            props.body->SetLinearVelocity(b2Vec2{car.velocity.x, car.velocity.y});
                            props.body->SetAngularVelocity(static_cast<float32>(car.angularVelocity));
                            props.body->SetTransform(b2Vec2{car.position.x, car.position.y},
                                                     static_cast<float32>(car.heading));
                        }
                    }
                }
            }

            // Handle general collision
            {
                auto view = manager.createView<component::WorldP3D, component::Tag_Dynamic,
                        component::PhysicalProperties>();
                for (auto entity: view) {
                    auto &props = view.get<component::PhysicalProperties>(entity);
                    auto &pos = view.get<component::WorldP3D>(entity);

                    mCollisionHandlers[props.physicalCategory](entityFactory,
                                                               entity,
                                                               props,
                                                               pos);
                }
            }

            // Sync placement with box2d
            {
                auto view = manager.createView<component::WorldP3D, component::Tag_Dynamic, component::Heading, component::Scale,
                        component::PhysicalProperties>();

                for (auto entity: view) {
                    auto &props = view.get<component::PhysicalProperties>(entity);
                    auto &position = props.body->GetPosition();
                    auto &pos = view.get<component::WorldP3D>(entity);
                    auto &heading = view.get<component::Heading>(entity);
                    auto &scale = view.get<component::Scale>(entity);

                    if (std::abs(pos.x - position.x) > common::EP ||
                        std::abs(pos.y - position.y) > common::EP ||
                        std::abs(heading.value - props.body->GetAngle()) > common::EP) {
                        if (manager.has<component::Trail>(entity)) {
                            auto &trail = manager.get<component::Trail>(entity);
                            trail.previousPos.push_back(pos);
                            trail.velocity.push_back(props.body->GetLinearVelocity().Length());
                            assert(trail.previousPos.size() == trail.velocity.size());
                        }

                        pos.x = position.x;
                        pos.y = position.y;
                        heading.value = props.body->GetAngle();
                        updateTransforms(manager, entity, pos, heading, scale);
                        entitiesToBeUpdated.push_back(entity);
                    }
                }
            }

            // Update tires
            {
                auto view = manager.createView<component::EntityAttachment, component::Car>();
                for (auto &&entity : view) {
                    const auto &attachments = view.get<component::EntityAttachment>(entity);
                    const auto &car = view.get<component::Car>(entity);
                    for (common::size i = 0; i < attachments.entities.size(); ++i) {
                        if (attachments.entityTypes[i] == entities::EntityType::VEHICLE_TIRE_FRONT) {
                            auto &heading = manager.get<component::Heading>(attachments.entities[i]).value;

                            // TODO: I shouldn't need to do this kinda of rotation transformation, x-1.0f + 90.0f.
                            // There seems to be something wrong with the way tires are created in the beginning
                            heading = static_cast<oni::common::real32>(car.steerAngle +
                                                                       math::toRadians(90.0f));
                        }
                    }
                }
            }

            // Update Projectiles
            {
                mProjectile->tick(entityFactory, clientData, tickTime);
            }

            // Update age
            {
                auto policy = entities::EntityOperationPolicy{false, false};
                policy.safe = false;
                policy.track = true;
                updateAge(entityFactory, tickTime, policy);
            }

            // Tag to sync with client
            {
                for (auto &&entity : entitiesToBeUpdated) {
                    manager.tagForComponentSync(entity);
                }
            }
        }

        b2World *
        Dynamics::getPhysicsWorld() {
            return mPhysicsWorld.get();
        }

        bool
        Dynamics::isColliding(b2Body *body) {
            bool colliding{false};
            for (b2ContactEdge *ce = body->GetContactList();
                 ce && !colliding; ce = ce->next) {
                b2Contact *c = ce->contact;
//                if (c->GetFixtureA()->IsSensor() || c->GetFixtureB()->IsSensor()) {
                colliding = c->IsTouching();
//                }
            }
            return colliding;
        }

        void
        Dynamics::handleRocketCollision(entities::EntityFactory &entityFactory,
                                        common::EntityID entityID,
                                        component::PhysicalProperties &props,
                                        component::WorldP3D &pos) {

            if (isColliding(props.body)) {
                // TODO: Proper Z level!
                common::real32 particleZ = 0.25f; //mZLevel.level_2 + mZLevel.majorLevelDelta;
                auto worldPos = component::WorldP3D{};
                worldPos.x = props.body->GetPosition().x;
                worldPos.y = props.body->GetPosition().y;
                worldPos.z = particleZ;

                auto colliding = game::CollidingEntity{};
                colliding.entityA = entities::EntityType::SIMPLE_ROCKET;
                // TODO: use the proper type for the other entity instead of UNKNOWN
                colliding.entityB = entities::EntityType::UNKNOWN;

                // NOTE: It is up to the client to decide what to do with this event, such as spawning particles, playing
                // sound effects, etc.
                entityFactory.getEntityManager().enqueueEvent<game::Event_Collision>(worldPos, colliding);

                entityFactory.removeEntity(entityID, entities::EntityOperationPolicy{true, false});
                // TODO: I'm leaking memory here, data in b2world is left behind :(
                // TODO: How can I make an interface that makes this impossible? I don't want to remember everytime
                // that I removeEntity that I also have to delete it from other places, such as b2world, textures,
                // and audio system.
            }
        }

        void
        Dynamics::updateTransforms(entities::EntityManager &manager,
                                   common::EntityID entity,
                                   const component::WorldP3D &pos,
                                   const component::Heading &heading,
                                   const component::Scale &scale) {
            if (manager.has<component::TransformChildren>(entity)) {
                auto transformChildren = manager.get<component::TransformChildren>(entity);
                for (auto child: transformChildren.children) {
                    auto transformParent = manager.get<component::TransformParent>(child);
                    transformParent.transform = math::createTransformation(pos, heading, scale);

                    updateTransformParent(manager, child, transformParent);
                }
            }
        }

        void
        Dynamics::updateTransformParent(entities::EntityManager &manager,
                                        common::EntityID entity,
                                        const component::TransformParent &transformParent) {
            // TODO: This function should recurse
            manager.replace<component::TransformParent>(entity, transformParent);
            manager.tagForComponentSync(entity);
        }

        void
        Dynamics::tickClientSide(entities::EntityFactory &entityFactory,
                                 common::real64 tickTime,
                                 const entities::EntityOperationPolicy &policy) {
            updateAge(entityFactory, tickTime, policy);
            updatePlacement(entityFactory, tickTime, policy);
        }

        void
        Dynamics::updateAge(entities::EntityFactory &entityFactory,
                            common::real64 tickTime,
                            const entities::EntityOperationPolicy &policy) {
            auto &entityManager = entityFactory.getEntityManager();
            // TODO: NOT very happy with this design. In essence I can't just look at Age component, and remove entities with age > maxAge.
            // I need more information to act on it. But as is now this requires so many components that it is hard to keep it in sync with other systems.
            // Maybe it is okay to have it as is. But think about the future when you have 10 different behaviours for on death effect, how would you
            // partition the components space to select the right entities to handle the effects in each loop block?
            // TODO: One thing that makes this code complicated is the fact that Particles are special type of entities where the engine doesn't
            // know about WorldP3D, the shader calculates their location only at draw time but when they die the effect has to take place
            // where the shader last drawn it, so I have to calculate the same position again here. If WorlP3D was correct, the loop would have
            // been much simpler and more generic, meaning, I can just look at Age, and create the event for the pos if it has Tag_SplatOnDeath.
            // And the problem with this special case is that the fact that I am processing Particles, a given EntityType, is implicit, but the
            // code masquerades itself as generic. So that is messed up.
            auto view = entityManager.createView<component::Age, component::WorldP3D, component::Tessellation, component::Heading, component::Velocity>();
            for (const auto &entity: view) {
                auto &age = view.get<component::Age>(entity);
                age.currentAge += tickTime;
                if (age.currentAge > age.maxAge) {
                    if (entityManager.has<component::Tag_SplatOnDeath>(entity)) {
                        auto &pos = view.get<component::WorldP3D>(entity);
                        auto &tess = view.get<component::Tessellation>(entity);
                        auto &heading = view.get<component::Heading>(entity);
                        auto &velocity = view.get<component::Velocity>(entity);
                        pos.x += std::cos(heading.value) * velocity.currentVelocity;
                        pos.y += std::sin(heading.value) * velocity.currentVelocity;

                        auto size = component::Size{tess.halfSize, tess.halfSize};
                        auto &texture = entityManager.get<component::Texture>(entity);
                        entityManager.enqueueEvent<game::Event_SplatOnDeath>(pos, size, texture.filePath);
                    }
                    entityFactory.removeEntity(entity, policy);
                }
            }
        }

        void
        Dynamics::updatePlacement(entities::EntityFactory &entityFactory,
                                  common::real64 tickTime,
                                  const entities::EntityOperationPolicy &policy) {
            /// Update particle placement
            {
                auto view = entityFactory.getEntityManager().createView<component::WorldP3D, component::Velocity, component::Heading, component::Age, component::Tag_EngineOnlyParticlePhysics>();
                for (const auto &entity: view) {
                    auto &pos = view.get<component::WorldP3D>(entity).value;
                    const auto &velocity = view.get<component::Velocity>(entity);
                    const auto &age = view.get<component::Age>(entity);
                    const auto &heading = view.get<component::Heading>(entity).value;

                    auto currentVelocity =
                            5 * (velocity.currentVelocity * tickTime) - math::pow(age.currentAge, 10) * 0.5f;
                    math::zeroClip(currentVelocity);

                    common::real32 x = std::cos(heading) * currentVelocity;
                    common::real32 y = std::sin(heading) * currentVelocity;

                    pos.x += x;
                    pos.y += y;
                }
            }
        }

        void
        Dynamics::handleCollision(entities::EntityFactory &,
                                  common::EntityID,
                                  component::PhysicalProperties &,
                                  component::WorldP3D &) {

        }

        void
        Dynamics::handleRaceCarCollision(entities::EntityFactory &,
                                         common::EntityID,
                                         component::PhysicalProperties &,
                                         component::WorldP3D &) {

        }

        void
        Dynamics::handleVehicleCollision(entities::EntityFactory &,
                                         common::EntityID,
                                         component::PhysicalProperties &,
                                         component::WorldP3D &) {

        }
    }
}