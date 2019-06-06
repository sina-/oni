#include <oni-core/physics/oni-physics-dynamics.h>

#include <Box2D/Box2D.h>
#include <GLFW/glfw3.h>

#include <oni-core/entities/oni-entities-manager.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-physics.h>
#include <oni-core/component/oni-component-visual.h>
#include <oni-core/common/oni-common-const.h>
#include <oni-core/math/oni-math-rand.h>
#include <oni-core/graphic/oni-graphic-window.h>
#include <oni-core/physics/oni-physics-car.h>
#include <oni-core/math/oni-math-transformation.h>
#include <oni-core/physics/oni-physics-projectile.h>
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

        Dynamics::Dynamics() {
            b2Vec2 gravity(0.0f, 0.0f);
            mCollisionHandler = std::make_unique<CollisionHandler>();
            mPhysicsWorld = std::make_unique<b2World>(gravity);
            mProjectile = std::make_unique<Projectile>(mPhysicsWorld.get());
            mRand = std::make_unique<math::Rand>(0, 0);

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
        Dynamics::tick(entities::EntityManager &manager,
                       entities::ClientDataManager *clientData,
                       common::r64 tickTime) {
            std::map<common::EntityID, io::CarInput> carInput{};

            /// Input
            {
                auto carView = manager.createView<
                        component::Tag_SimModeServer,
                        component::WorldP3D,
                        component::Heading,
                        component::Scale,
                        component::Car,
                        component::CarConfig>();
                for (auto &&id: carView) {
                    auto input = clientData->getClientInput(id);
                    // NOTE: This could happen just at the moment when a client joins, an entity is created by the
                    // client data structures are not initialized.
                    if (!input) {
                        continue;
                    }

                    auto &car = carView.get<component::Car>(id);
                    auto &heading = carView.get<component::Heading>(id);
                    const auto &carConfig = carView.get<component::CarConfig>(id);

                    if (input->isPressed(GLFW_KEY_W) || input->isPressed(GLFW_KEY_UP)) {
                        // TODO: When using game-pad, this value will vary between (0.0f...1.0f)
                        carInput[id].throttle = 1.0f;
                    }
                    if (input->isPressed(GLFW_KEY_A) || input->isPressed(GLFW_KEY_LEFT)) {
                        carInput[id].left = 1.0f;
                    }
                    if (input->isPressed(GLFW_KEY_S) || input->isPressed(GLFW_KEY_DOWN)) {
                        carInput[id].throttle = -1.0f;
                    }
                    if (input->isPressed(GLFW_KEY_D) || input->isPressed(GLFW_KEY_RIGHT)) {
                        carInput[id].right = 1.0f;
                    }
                    if (input->isPressed(GLFW_KEY_LEFT_SHIFT)) {
                        car.velocity = car.velocity + math::vec2{static_cast<common::r32>(cos(heading.value)),
                                                                 static_cast<common::r32>(sin(heading.value))};
                    }
                    if (input->isPressed(GLFW_KEY_SPACE)) {
                        if (car.accumulatedEBrake < 1.0f) {
                            car.accumulatedEBrake += 0.01f;
                        }
                        carInput[id].eBrake = static_cast<common::r32>(car.accumulatedEBrake);
                    } else {
                        car.accumulatedEBrake = 0.0f;
                    }

                    auto steerInput = carInput[id].left - carInput[id].right;
                    if (car.smoothSteer) {
                        car.steer = applySmoothSteer(car, steerInput, tickTime);
                    } else {
                        car.steer = steerInput;
                    }


                    if (car.safeSteer) {
                        car.steer = applySafeSteer(car, steerInput);
                    }

                    car.steerAngle = car.steer * carConfig.maxSteer;

                    auto &carPos = carView.get<component::WorldP3D>(id);
                    auto &scale = carView.get<component::Scale>(id);

                    const auto oldPos = carPos;
                    const auto oldHeading = heading;
                    tickCar(car, carPos, heading, carConfig, carInput[id], tickTime);

                    auto velocity = car.velocityLocal.len();
                    auto distanceFromCamera = 1 + velocity * 2 / car.maxVelocityAbsolute;
                    if (!math::almost_Equal(oldPos.x, carPos.x) ||
                        !math::almost_Equal(oldPos.y, carPos.y) ||
                        !math::almost_Equal(oldPos.y, carPos.y) ||
                        !math::almost_Equal(oldHeading.value, heading.value)) {

                        car.distanceFromCamera = distanceFromCamera;

                        manager.markForNetSync(id);
                    }
                }
            }

            /// Update box2d world
            {
                // TODO: entity registry has pointers to mPhysicsWorld internal data structures :(
                // One way to hide it is to provide a function in physics library that creates physical entities
                // for a given entity id an maintains an internal mapping between them without leaking the
                // implementation to outside.
                mPhysicsWorld->Step(tickTime, 6, 2);
            }


            /// Car collisions
            {
                auto view = manager.createView<
                        component::Tag_SimModeServer,
                        component::Car,
                        component::Heading,
                        component::WorldP3D,
                        component::PhysicalProperties>();
                for (auto &&id: view) {
                    auto &props = view.get<component::PhysicalProperties>(id);
                    auto &car = view.get<component::Car>(id);
                    auto &pos = view.get<component::WorldP3D>(id);
                    auto &heading = view.get<component::Heading>(id);
                    auto *body = manager.getEntityBody(id);
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
                        body->ApplyForceToCenter(
                                b2Vec2(static_cast<common::r32>(std::cos(heading.value) * 30 *
                                                                carInput[id].throttle),
                                       static_cast<common::r32>(std::sin(heading.value) * 30 *
                                                                carInput[id].throttle)),
                                true);
                        car.isColliding = false;
                    } else {
                        if (isColliding(body)) {
                            car.velocity = math::vec2{body->GetLinearVelocity().x,
                                                      body->GetLinearVelocity().y};
                            car.angularVelocity = body->GetAngularVelocity();
                            pos.x = body->GetPosition().x;
                            pos.y = body->GetPosition().y;
                            heading.value = body->GetAngle();
                            car.isColliding = true;
                        } else {
                            body->SetLinearVelocity(b2Vec2{car.velocity.x, car.velocity.y});
                            body->SetAngularVelocity(static_cast<float32>(car.angularVelocity));
                            body->SetTransform(b2Vec2{pos.x, pos.y},
                                               static_cast<float32>(heading.value));
                        }
                    }
                }
            }

            /// General collision
            {
                auto view = manager.createView<
                        component::Tag_SimModeServer,
                        component::Tag_Dynamic,
                        component::WorldP3D,
                        component::PhysicalProperties>();
                for (auto &&id: view) {
                    auto &props = view.get<component::PhysicalProperties>(id);
                    auto &pos = view.get<component::WorldP3D>(id);

                    mCollisionHandlers[props.physicalCategory](manager,
                                                               id,
                                                               props,
                                                               pos);
                }
                manager.flushDeletions();
            }

            /// Sync placement with box2d
            {
                auto view = manager.createView<
                        component::Tag_SimModeServer,
                        component::Tag_Dynamic,
                        component::WorldP3D,
                        component::Heading,
                        component::Scale,
                        component::PhysicalProperties>();

                for (auto &&id: view) {
                    auto *body = manager.getEntityBody(id);
                    auto &props = view.get<component::PhysicalProperties>(id);
                    auto &position = body->GetPosition();
                    auto &pos = view.get<component::WorldP3D>(id);
                    auto &heading = view.get<component::Heading>(id);
                    auto &scale = view.get<component::Scale>(id);

                    if (std::abs(pos.x - position.x) > common::EP32 ||
                        std::abs(pos.y - position.y) > common::EP32 ||
                        std::abs(heading.value - body->GetAngle()) > common::EP32) {
                        if (manager.has<component::Trail>(id)) {
                            auto &trail = manager.get<component::Trail>(id);
                            trail.previousPos.push_back(pos);
                            trail.velocity.push_back(body->GetLinearVelocity().Length());
                            assert(trail.previousPos.size() == trail.velocity.size());
                        }

                        pos.x = position.x;
                        pos.y = position.y;
                        heading.value = body->GetAngle();
                        manager.markForNetSync(id);
                    }
                }
            }

            /// Update tires
            {
                auto view = manager.createView<
                        component::Tag_SimModeServer,
                        component::EntityAttachment,
                        component::Car>();
                for (auto &&id : view) {
                    const auto &attachments = view.get<component::EntityAttachment>(id);
                    const auto &car = view.get<component::Car>(id);
                    for (common::size i = 0; i < attachments.entities.size(); ++i) {
                        if (attachments.entityTypes[i] == entities::EntityType::VEHICLE_TIRE_FRONT) {
                            auto &heading = manager.get<component::Heading>(attachments.entities[i]).value;

                            // TODO: I shouldn't need to do this kinda of rotation transformation, x-1.0f + 90.0f.
                            // There seems to be something wrong with the way tires are created in the beginning
                            heading = static_cast<oni::common::r32>(car.steerAngle +
                                                                    math::toRadians(90.0f));

                            manager.markForNetSync(attachments.entities[i]);
                        }
                    }
                }
            }

            /// Update Projectiles
            {
                mProjectile->tick(manager, clientData, tickTime);
            }

            /// Update age
            {
                updateAge(manager, tickTime);
            }

            /// Update placement
            {
                updatePlacement(manager, tickTime);
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
        Dynamics::handleRocketCollision(entities::EntityManager &manager,
                                        common::EntityID id,
                                        component::PhysicalProperties &props,
                                        component::WorldP3D &pos) {

            auto *body = manager.getEntityBody(id);
            if (isColliding(body)) {
                // TODO: Proper Z level!
                common::r32 particleZ = 0.25f; //mZLevel.level_2 + mZLevel.majorLevelDelta;
                auto worldPos = component::WorldP3D{};
                worldPos.x = body->GetPosition().x;
                worldPos.y = body->GetPosition().y;
                worldPos.z = particleZ;

                auto colliding = game::CollidingEntity{};
                colliding.entityA = entities::EntityType::SIMPLE_ROCKET;
                // TODO: use the proper type for the other entity instead of UNKNOWN
                colliding.entityB = entities::EntityType::UNKNOWN;

                // NOTE: It is up to the client to decide what to do with this event, such as spawning particles, playing
                // sound effects, etc.
                manager.enqueueEvent<game::Event_Collision>(worldPos, colliding);

                manager.markForDeletion(id);
                // TODO: I'm leaking memory here, data in b2world is left behind :(
                // TODO: How can I make an interface that makes this impossible? I don't want to remember everytime
                // that I removeEntity that I also have to delete it from other places, such as b2world, textures,
                // and audio system.
            }
        }

        void
        Dynamics::updateAge(entities::EntityManager &manager,
                            common::r64 tickTime) {
            {
                /// Client side
                auto view = manager.createView<
                        component::Tag_SimModeClient,
                        component::Age>();
                for (auto &&id: view) {
                    auto &age = view.get<component::Age>(id);
                    age.currentAge += tickTime;
                    if (age.currentAge > age.maxAge) {
                        if (manager.has<component::Tag_SplatOnDeath>(id)) {
                            auto &pos = manager.get<component::WorldP3D>(id);
                            auto &size = manager.get<component::Scale>(id);
                            auto &tag = manager.get<component::TextureTag>(id);

                            manager.enqueueEvent<game::Event_SplatOnDeath>(pos, size, tag);
                        }
                        manager.markForDeletion(id);
                    }
                }
            }
            /// Server
            {
                auto view = manager.createView<
                        component::Tag_SimModeServer,
                        component::Age>();
                for (auto &&id: view) {
                    auto &age = view.get<component::Age>(id);
                    age.currentAge += tickTime;
                    manager.markForNetSync(id);
                    if (age.currentAge > age.maxAge) {
                        manager.markForDeletion(id);
                    }
                }
            }
            manager.flushDeletions();
        }

        void
        Dynamics::updatePlacement(entities::EntityManager &manager,
                                  common::r64 tickTime) {
            /// Update particle placement
            /// Client
            {
                auto view = manager.createView<
                        component::Tag_SimModeClient,
                        component::WorldP3D,
                        component::Velocity,
                        component::Heading,
                        component::Age>();
                for (auto &&id: view) {
                    auto &pos = view.get<component::WorldP3D>(id);
                    const auto &velocity = view.get<component::Velocity>(id);
                    const auto &age = view.get<component::Age>(id);
                    const auto &heading = view.get<component::Heading>(id).value;

                    auto currentVelocity =
                            5 * (velocity.currentVelocity * tickTime) - math::pow(age.currentAge, 10) * 0.5f;
                    math::zeroClip(currentVelocity);

                    common::r32 x = std::cos(heading) * currentVelocity;
                    common::r32 y = std::sin(heading) * currentVelocity;

                    pos.x += x;
                    pos.y += y;
                }
            }
        }

        void
        Dynamics::handleCollision(entities::EntityManager &,
                                  common::EntityID,
                                  component::PhysicalProperties &,
                                  component::WorldP3D &) {

        }

        void
        Dynamics::handleRaceCarCollision(entities::EntityManager &,
                                         common::EntityID,
                                         component::PhysicalProperties &,
                                         component::WorldP3D &) {

        }

        void
        Dynamics::handleVehicleCollision(entities::EntityManager &,
                                         common::EntityID,
                                         component::PhysicalProperties &,
                                         component::WorldP3D &) {

        }
    }
}