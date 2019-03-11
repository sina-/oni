#include <oni-core/physics/dynamics.h>

#include <Box2D/Box2D.h>
#include <GLFW/glfw3.h>

#include <oni-core/entities/entity-manager.h>
#include <oni-core/component/geometry.h>
#include <oni-core/component/physic.h>
#include <oni-core/component/visual.h>
#include <oni-core/common/consts.h>
#include <oni-core/math/rand.h>
#include <oni-core/graphic/window.h>
#include <oni-core/physics/car.h>
#include <oni-core/math/transformation.h>
#include <oni-core/physics/projectile.h>
#include <oni-core/entities/entity-factory.h>
#include <oni-core/component/hierarchy.h>

namespace oni {
    namespace physics {
        class CollisionHandler : public b2ContactListener {
        public:
            void BeginContact(b2Contact *contact) override {
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

            void EndContact(b2Contact *contact) override {
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

            mCollisionHandlers[component::PhysicalCategory::BULLET] =
                    std::bind(&Dynamics::handleBulletCollision, this,
                              std::placeholders::_1,
                              std::placeholders::_2,
                              std::placeholders::_3,
                              std::placeholders::_4
                    );

            mCollisionHandlers[component::PhysicalCategory::VEHICLE] =
                    std::bind(&Dynamics::handleVehicleCollision, this,
                              std::placeholders::_1,
                              std::placeholders::_2,
                              std::placeholders::_3,
                              std::placeholders::_4
                    );

            mCollisionHandlers[component::PhysicalCategory::RACE_CAR] =
                    std::bind(&Dynamics::handleRaceCarCollision, this,
                              std::placeholders::_1,
                              std::placeholders::_2,
                              std::placeholders::_3,
                              std::placeholders::_4
                    );

            mCollisionHandlers[component::PhysicalCategory::WALL] =
                    std::bind(&Dynamics::handleCollision, this,
                              std::placeholders::_1,
                              std::placeholders::_2,
                              std::placeholders::_3,
                              std::placeholders::_4
                    );

            mCollisionHandlers[component::PhysicalCategory::GENERIC] =
                    std::bind(&Dynamics::handleCollision, this,
                              std::placeholders::_1,
                              std::placeholders::_2,
                              std::placeholders::_3,
                              std::placeholders::_4
                    );
            // TODO: Can't get this working, its unreliable, when there are lot of collisions in the world, it keeps
            // skipping some of them!
            // mPhysicsWorld->SetContactListener(mCollisionHandler.get());
        }

        Dynamics::~Dynamics() = default;

        void Dynamics::tick(entities::EntityFactory &entityFactory,
                            entities::ClientDataManager &clientData,
                            common::real64 tickTime) {
            auto &manager = entityFactory.getEntityManager();
            std::map<common::EntityID, component::CarInput> carInput{};
            std::vector<common::EntityID> entitiesToBeUpdated{};
            {
                // NOTE: Need to lock it because network system might remove cars for clients that have disconnected.
                // TODO: Maybe there is a better way to tick the cars without needing to lock the whole registry!
                // One solutions could be, if this loop is very heavy, is to create a copy of all the data I need to
                // operate on and do all the calculation and then lock the registry and update all the corresponding
                // entities.
                auto carView = manager.createViewScopeLock<component::Placement, component::Car, component::CarConfig>();

                for (auto &&entity: carView) {
                    auto clientLock = clientData.scopedLock();
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

                    auto &carPlacement = carView.get<component::Placement>(entity);
                    carPlacement.position.x = car.position.x;
                    carPlacement.position.y = car.position.y;
                    carPlacement.rotation = static_cast<common::real32>(car.heading);
                    updateTransforms(manager, entity, carPlacement);

                    auto velocity = car.velocityLocal.len();
                    car.distanceFromCamera = 1 + velocity * 2 / car.maxVelocityAbsolute;

                    entitiesToBeUpdated.push_back(entity);
                }
            }

            {
                // NOTE: Step() function will modify collision status of entities, so we have to lock the registry.
                auto lock = manager.scopedLock();
                // TODO: entity registry has pointers to mPhysicsWorld internal data structures :(
                // One way to hide it is to provide a function in physics library that creates physical entities
                // for a given entity id an maintains an internal mapping between them without leaking the
                // implementation to outside.
                mPhysicsWorld->Step(mTickFrequency, 6, 2);
            }

            mProjectile->tick(entityFactory, clientData, tickTime);

            {
                auto carPhysicsView = manager.createViewScopeLock<component::Car, component::PhysicalProperties>();

                // Handle collision
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
                        if (auto colliding = isColliding(props.body)) {
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

            {
                // Handle collision
                auto view = manager.createViewScopeLock<component::Placement, component::Tag_Dynamic,
                        component::PhysicalProperties>();
                for (auto entity: view) {
                    auto &props = view.get<component::PhysicalProperties>(entity);
                    auto &placement = view.get<component::Placement>(entity);

                    mCollisionHandlers[props.physicalCategory](entityFactory,
                                                               entity,
                                                               props, placement);
                }
            }
            {
                // Update placement
                auto view = manager.createViewScopeLock<component::Placement, component::Tag_Dynamic,
                        component::PhysicalProperties>();

                for (auto entity: view) {
                    auto &props = view.get<component::PhysicalProperties>(entity);
                    auto &position = props.body->GetPosition();
                    auto &placement = view.get<component::Placement>(entity);

                    if (std::abs(placement.position.x - position.x) > common::EP ||
                        std::abs(placement.position.y - position.y) > common::EP ||
                        std::abs(placement.rotation - props.body->GetAngle()) > common::EP) {
                        if (manager.has<component::Trail>(entity)) {
                            auto &trail = manager.get<component::Trail>(entity);
                            trail.previousPos.push_back(placement.position);
                            trail.velocity.push_back(props.body->GetLinearVelocity().Length());
                            assert(trail.previousPos.size() == trail.velocity.size());
                        }

                        placement.position.x = position.x;
                        placement.position.y = position.y;
                        placement.rotation = props.body->GetAngle();
                        updateTransforms(manager, entity, placement);
                        entitiesToBeUpdated.push_back(entity);
                    }
                }
            }

            {
                // Update tires
                auto view = manager.createViewScopeLock<component::EntityAttachment, component::Car>();
                for (auto &&entity : view) {
                    const auto &attachments = view.get<component::EntityAttachment>(entity);
                    const auto &car = view.get<component::Car>(entity);
                    for (common::size i = 0; i < attachments.entities.size(); ++i) {
                        if (attachments.entityTypes[i] == component::EntityType::VEHICLE_TIRE_FRONT) {
                            auto &tirePlacement = manager.get<component::Placement>(attachments.entities[i]);

                            // TODO: I shouldn't need to do this kinda of rotation transformation, x-1.0f + 90.0f.
                            // There seems to be something wrong with the way tires are created in the beginning
                            tirePlacement.rotation = static_cast<oni::common::real32>(car.steerAngle +
                                                                                      math::toRadians(90.0f));
                        }
                    }
                }
            }

            {
                auto lock = manager.scopedLock();
                for (auto &&entity : entitiesToBeUpdated) {
                    manager.tagForComponentSync(entity);
                }
            }
        }

        b2World *Dynamics::getPhysicsWorld() {
            return mPhysicsWorld.get();
        }

        bool Dynamics::isColliding(b2Body *body) {
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

        void Dynamics::handleBulletCollision(entities::EntityFactory &entityFactory,
                                             common::EntityID entity,
                                             component::PhysicalProperties &props,
                                             component::Placement &placement) {

            if (isColliding(props.body)) {
                // TODO: Proper Z level!
                common::real32 particleZ = 0.25f; //mZLevel.level_2 + mZLevel.majorLevelDelta;
                math::vec3 pos{props.body->GetPosition().x, props.body->GetPosition().y, particleZ};

                entityFactory.createEvent<component::EventType::COLLISION>(component::EntityType::SIMPLE_BULLET,
                        // TODO: use the proper type for the other entity instead of UNKNOWN
                                                                           component::EntityType::UNKNOWN,
                                                                           pos);
                entityFactory.removeEntity(entity, true, false);
            }
        }

        void
        Dynamics::handleVehicleCollision(entities::EntityFactory &entityFactory,
                                         common::EntityID entity,
                                         component::PhysicalProperties &props,
                                         component::Placement &placement) {
        }

        void Dynamics::handleCollision(entities::EntityFactory &entityFactory,
                                       common::EntityID entity,
                                       component::PhysicalProperties &props,
                                       component::Placement &placement) {
        }

        void
        Dynamics::handleRaceCarCollision(entities::EntityFactory &entityFactory,
                                         common::EntityID entity,
                                         component::PhysicalProperties &props,
                                         component::Placement &placement) {
        }

        void Dynamics::updateTransforms(entities::EntityManager &manager,
                                        common::EntityID entity,
                                        const component::Placement &placement) {
            if (manager.has<component::TransformChildren>(entity)) {
                auto transformChildren = manager.get<component::TransformChildren>(entity);
                for (auto child: transformChildren.children) {
                    auto transformParent = manager.get<component::TransformParent>(child);
                    transformParent.transform = math::Transformation::createTransformation(placement.position,
                                                                                           placement.rotation,
                                                                                           placement.scale);

                    updateTransformParent(manager, child, transformParent);
                }
            }
        }

        void Dynamics::updateTransformParent(entities::EntityManager &manager,
                                             common::EntityID entity,
                                             const component::TransformParent &transformParent) {
            // TODO: This function should recurse
            manager.replace<component::TransformParent>(entity, transformParent);
            manager.tagForComponentSync(entity);
        }

    }

}