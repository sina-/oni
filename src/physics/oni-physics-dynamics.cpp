#include <oni-core/physics/oni-physics-dynamics.h>

#include <Box2D/Box2D.h>
#include <GLFW/glfw3.h>

#include <oni-core/entities/oni-entities-manager.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-physics.h>
#include <oni-core/component/oni-component-visual.h>
#include <oni-core/common/oni-common-const.h>
#include <oni-core/math/oni-math-rand.h>
#include <oni-core/physics/oni-physics-car.h>
#include <oni-core/game/oni-game-event.h>
#include <oni-core/component/oni-component-gameplay.h>


namespace oni {
#if 0
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
#endif

    Dynamics::Dynamics() {
        auto gravity = b2Vec2(0.0f, 0.0f);
        ///mCollisionHandler = std::make_unique<CollisionHandler>();
        mPhysicsWorld = std::make_unique<b2World>(gravity);
        mRand = std::make_unique<Rand>(0, 0);

        mCollisionHandlers[PhysicalCategory::ROCKET] =
                std::bind(&Dynamics::handleRocketCollision, this,
                          std::placeholders::_1,
                          std::placeholders::_2,
                          std::placeholders::_3,
                          std::placeholders::_4);

        mCollisionHandlers[PhysicalCategory::VEHICLE] =
                std::bind(&Dynamics::handleVehicleCollision, this,
                          std::placeholders::_1,
                          std::placeholders::_2,
                          std::placeholders::_3,
                          std::placeholders::_4);

        mCollisionHandlers[PhysicalCategory::RACE_CAR] =
                std::bind(&Dynamics::handleRaceCarCollision, this,
                          std::placeholders::_1,
                          std::placeholders::_2,
                          std::placeholders::_3,
                          std::placeholders::_4);

        mCollisionHandlers[PhysicalCategory::WALL] =
                std::bind(&Dynamics::handleCollision, this,
                          std::placeholders::_1,
                          std::placeholders::_2,
                          std::placeholders::_3,
                          std::placeholders::_4);

        mCollisionHandlers[PhysicalCategory::GENERIC] =
                std::bind(&Dynamics::handleCollision, this,
                          std::placeholders::_1,
                          std::placeholders::_2,
                          std::placeholders::_3,
                          std::placeholders::_4);

        mCollisionHandlers[PhysicalCategory::PROJECTILE] =
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
    Dynamics::processInput(EntityManager &manager,
                           ClientDataManager &clientData,
                           std::unordered_map<EntityID, CarInput> &result) {
        assert(manager.getSimMode() == SimMode::SERVER);

        auto carView = manager.createView<
                Heading,
                Car,
                CarConfig>();
        for (auto &&id: carView) {
            auto input = clientData.getClientInput(id);
            auto &car = carView.get<Car>(id);
            auto &heading = carView.get<Heading>(id);
            const auto &carConfig = carView.get<CarConfig>(id);
            constexpr r32 steeringSensitivity = 0.9f;

            if (input->isPressed(GLFW_KEY_W) || input->isPressed(GLFW_KEY_UP)) {
                // TODO: When using game-pad, this value will vary between (0.0f...1.0f)
                result[id].throttle = 1.0f;
            }
            if (input->isPressed(GLFW_KEY_A) || input->isPressed(GLFW_KEY_LEFT)) {
                result[id].left = steeringSensitivity;
            }
            if (input->isPressed(GLFW_KEY_S) || input->isPressed(GLFW_KEY_DOWN)) {
                result[id].throttle = -1.0f;
            }
            if (input->isPressed(GLFW_KEY_D) || input->isPressed(GLFW_KEY_RIGHT)) {
                result[id].right = steeringSensitivity;
            }
            if (input->isPressed(GLFW_KEY_LEFT_SHIFT)) {
                car.velocity = car.velocity + vec2{static_cast<r32>(cos(heading.value)),
                                                   static_cast<r32>(sin(heading.value))};
            }
            if (input->isPressed(GLFW_KEY_SPACE)) {
                result[id].eBrake = 1.f;
            }
        }
    }

    void
    Dynamics::updateCars(EntityManager &manager,
                         UserInputMap &input,
                         r64 tickTime) {
        assert(manager.getSimMode() == SimMode::SERVER);

        /// Update car
        {
            auto carView = manager.createView<
                    WorldP3D,
                    Heading,
                    Scale,
                    Car,
                    CarConfig>();
            for (auto &&id: carView) {
                auto &car = carView.get<Car>(id);
                auto &heading = carView.get<Heading>(id);
                const auto &carConfig = carView.get<CarConfig>(id);

                auto steerInput = input[id].left - input[id].right;
                if (car.smoothSteer) {
                    car.steer = applySmoothSteer(car, steerInput, tickTime);
                } else {
                    car.steer = steerInput;
                }


                if (car.safeSteer) {
                    car.steer = applySafeSteer(car, steerInput);
                }

                car.steerAngle = car.steer * carConfig.maxSteer;

                auto &carPos = carView.get<WorldP3D>(id);
                auto &scale = carView.get<Scale>(id);

                const auto oldPos = carPos;
                const auto oldHeading = heading;
                tickCar(car, carPos, heading, carConfig, input[id], tickTime);

                // TODO: This is probably not needed, client should be able to figure this out.
                auto velocity = car.velocityLocal.len();
                auto distanceFromCamera = 1 + velocity * 2 / car.maxVelocityAbsolute;
                if (!almost_Equal(oldPos.x, carPos.x) ||
                    !almost_Equal(oldPos.y, carPos.y) ||
                    !almost_Equal(oldPos.y, carPos.y) ||
                    !almost_Equal(oldHeading.value, heading.value)) {

                    car.distanceFromCamera = distanceFromCamera;

                    manager.markForNetSync(id);
                }
            }
        }

        /// Update tires
        {
            auto view = manager.createView<
                    EntityAttachment,
                    Car>();
            for (auto &&id : view) {
                const auto &attachments = view.get<EntityAttachment>(id);
                const auto &car = view.get<Car>(id);
                for (size i = 0; i < attachments.entities.size(); ++i) {
                    if (attachments.entityTypes[i] == EntityType::VEHICLE_TIRE_FRONT) {
                        auto &heading = manager.get<Heading>(attachments.entities[i]).value;

                        // TODO: I shouldn't need to do this kinda of rotation transformation, x-1.0f + 90.0f.
                        // There seems to be something wrong with the way tires are created in the beginning
                        heading = static_cast<r32>(car.steerAngle +
                                                   toRadians(90.0f));

                        manager.markForNetSync(attachments.entities[i]);
                    }
                }
            }
        }
    }

    void
    Dynamics::updateJetForce(EntityManager &manager,
                             r64 tickTime) {
        assert(manager.getSimMode() == SimMode::SERVER);

        auto emptyFuel = std::vector<EntityID>();
        auto view = manager.createView<
                JetForce,
                Heading,
                PhysicalProperties>();
        for (auto &&id: view) {
            auto *body = manager.getEntityBody(id);
            auto &heading = view.get<Heading>(id);
            auto &jet = view.get<JetForce>(id);
            if (!subAndZeroClip(jet.fuze, r32(tickTime))) {
                body->ApplyForceToCenter(
                        b2Vec2(cos(heading.value) * jet.force,
                               sin(heading.value) * jet.force),
                        true);
            } else {
                emptyFuel.push_back(id);
            }
        }
        for (auto &&id: emptyFuel) {
            manager.removeComponent<JetForce>(id);
        }
    }

    void
    Dynamics::updatePhysWorld(r64 tickTime) {
        // TODO: entity registry has pointers to mPhysicsWorld internal data structures :(
        // One way to hide it is to provide a function in physics library that creates physical entities
        // for a given entity id an maintains an internal mapping between them without leaking the
        // implementation to outside.
        mPhysicsWorld->Step(tickTime, 6, 2);
    }

    void
    Dynamics::updateCarCollision(EntityManager &manager,
                                 UserInputMap &input,
                                 r64 tickTime) {
        assert(manager.getSimMode() == SimMode::SERVER);

        auto view = manager.createView<
                Car,
                Heading,
                WorldP3D,
                WorldP3D_History,
                PhysicalProperties>();
        for (auto &&id: view) {
            auto &props = view.get<PhysicalProperties>(id);
            auto &car = view.get<Car>(id);
            auto &pos = view.get<WorldP3D>(id);
            auto &heading = view.get<Heading>(id);
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
                        b2Vec2(static_cast<r32>(std::cos(heading.value) * 30 *
                                                input[id].throttle),
                               static_cast<r32>(std::sin(heading.value) * 30 *
                                                input[id].throttle)),
                        true);
                car.isColliding = false;
            } else {
                if (isColliding(body)) {
                    car.velocity = vec2{body->GetLinearVelocity().x,
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
                    auto &hist = view.get<WorldP3D_History>(id);
                    hist.add(pos);
                }
            }
        }
    }

    void
    Dynamics::updateCollision(EntityManager &manager,
                              r64 tickTime) {
        assert(manager.getSimMode() == SimMode::SERVER ||
               manager.getSimMode() == SimMode::CLIENT);

        auto view = manager.createView<
                Tag_Dynamic,
                WorldP3D,
                PhysicalProperties>();
        for (auto &&id: view) {
            auto &props = view.get<PhysicalProperties>(id);
            auto &pos = view.get<WorldP3D>(id);

            auto *body = manager.getEntityBody(id);
            if (isColliding(body)) {
                mCollisionHandlers[props.physicalCategory](manager,
                                                           id,
                                                           props,
                                                           pos);
            }
        }
        manager.flushDeletions();
    }

    void
    Dynamics::syncPosWithPhysWorld(EntityManager &manager) {
        assert(manager.getSimMode() == SimMode::SERVER ||
               manager.getSimMode() == SimMode::CLIENT);

        auto view = manager.createView<
                Tag_Dynamic,
                WorldP3D,
                Heading,
                Scale,
                PhysicalProperties>();

        for (auto &&id: view) {
            auto *body = manager.getEntityBody(id);
            auto &bPos = body->GetPosition();
            auto &ePos = view.get<WorldP3D>(id);
            auto &heading = view.get<Heading>(id);
            auto &scale = view.get<Scale>(id);

            if (!almost_Equal(ePos.x, bPos.x) ||
                !almost_Equal(ePos.y, bPos.y) ||
                !almost_Equal(heading.value, body->GetAngle())) {

                ePos.x = bPos.x;
                ePos.y = bPos.y;

                if (manager.has<WorldP3D_History>(id)) {
                    manager.get<WorldP3D_History>(id).add(ePos);
                }

                heading.value = body->GetAngle();
                manager.markForNetSync(id);
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
    Dynamics::updateAge(EntityManager &manager,
                        r64 tickTime) {
        assert(manager.getSimMode() == SimMode::CLIENT ||
               manager.getSimMode() == SimMode::SERVER);

        auto view = manager.createView<
                Age>();
        for (auto &&id: view) {
            auto &age = view.get<Age>(id);
            age.currentAge += tickTime;
            if (age.currentAge > age.maxAge) {
                if (manager.has<Tag_SplatOnDeath>(id)) {
                    auto &pos = manager.get<WorldP3D>(id);
                    auto &size = manager.get<Scale>(id);
                    auto &tag = manager.get<EntityPreset>(id);
                    auto &heading = manager.get<Heading>(id);

                    auto callback = [&manager, id]() {
                        manager.deleteEntity(id);
                    };

                    manager.enqueueEvent<Event_SplatOnDeath>(pos, size, heading, tag, std::move(callback));
                } else {
                    manager.markForDeletion(id);
                }
            }
        }
        manager.flushDeletions();
    }

    void
    Dynamics::updateResting(EntityManager &manager) {
        assert(manager.getSimMode() == SimMode::CLIENT);

        auto view = manager.createView<
                PhysicalProperties,
                Scale,
                EntityPreset,
                WorldP3D,
                Heading,
                Tag_SplatOnRest>();
        for (auto &&id: view) {
            auto *body = manager.getEntityBody(id);
            if (!body->IsAwake()) {
                auto &pos = view.get<WorldP3D>(id);
                auto &size = view.get<Scale>(id);
                auto &tag = view.get<EntityPreset>(id);
                auto &heading = view.get<Heading>(id);

                auto callback = [&manager, id]() {
                    manager.deleteEntity(id);
                };
                // TODO: This will create copy for all. Good place for profiling and optimization as these entities
                // are often particles
                manager.enqueueEvent<Event_SplatOnRest>(pos, size, heading, tag, std::move(callback));
            }
        }
    }

    void
    Dynamics::updateFadeWithAge(EntityManager &manager,
                                r64 tickTime) {
        assert(manager.getSimMode() == SimMode::CLIENT ||
               manager.getSimMode() == SimMode::SERVER);

        auto view = manager.createView<
                Age,
                MaterialSkin,
                MaterialTransition_Fade>();
        for (auto &&id: view) {
            auto &age = view.get<Age>(id);
            auto &surface = view.get<MaterialSkin>(id);
            auto &fade = view.get<MaterialTransition_Fade>(id);

            auto targetAlpha = 1.f;
            switch (fade.fadeFunc) {
                case FadeFunc::LINEAR: {
                    targetAlpha = 1 - age.currentAge / age.maxAge;
                    break;
                }
                case FadeFunc::TAIL: {
                    constexpr auto dropOffT = 0.7f;
                    auto ageRatio = age.currentAge / age.maxAge;
                    if (ageRatio > dropOffT) {
                        targetAlpha = 1 - ageRatio;
                    }
                    break;
                }
                default: {
                    assert(false);
                    break;
                }
            }

            auto currentAlpha = surface.color.a_r32();
            surface.color.set_a(lerp(currentAlpha, targetAlpha, fade.factor));
            if (!almost_Equal(surface.color.a_r32(), currentAlpha)) {
                manager.markForNetSync(id);
            }
        }
    }

    void
    Dynamics::updateCoolDowns(EntityManager &manager,
                              r64 tickTime) {
        assert(manager.getSimMode() == SimMode::SERVER);

        /// Update cool-downs
        {
            // NOTE: This is not communicated to the clients so there are is no need for net-sync
            auto view = manager.createView<
                    GunCoolDown>();
            for (auto &&entity: view) {
                auto &coolDown = view.get<GunCoolDown>(entity);
                subAndZeroClip(coolDown.value, tickTime);
            }
        }
    }

    void
    Dynamics::handleRocketCollision(EntityManager &manager,
                                    EntityID id,
                                    PhysicalProperties &props,
                                    WorldP3D &pos) {

        auto *body = manager.getEntityBody(id);
        // TODO: Proper Z level!
        r32 particleZ = 0.25f; //mZLevel.level_2 + mZLevel.majorLevelDelta;
        auto worldPos = WorldP3D{};
        worldPos.x = body->GetPosition().x;
        worldPos.y = body->GetPosition().y;
        worldPos.z = particleZ;

        auto colliding = CollidingEntity{};
        colliding.entityA = EntityType::SIMPLE_ROCKET;
        // TODO: use the proper type for the other entity instead of UNKNOWN
        colliding.entityB = EntityType::UNKNOWN;

        // NOTE: It is up to the client to decide what to do with this event, such as spawning particles, playing
        // sound effects, etc.
        manager.enqueueEvent<Event_Collision>(worldPos, colliding);

        manager.markForDeletion(id);
        // TODO: I'm leaking memory here, data in b2world is left behind :(
        // TODO: How can I make an interface that makes this impossible? I don't want to remember everytime
        // that I removeEntity that I also have to delete it from other places, such as b2world, textures,
        // and audio system.
    }


    void
    Dynamics::handleCollision(EntityManager &,
                              EntityID,
                              PhysicalProperties &,
                              WorldP3D &) {

    }

    void
    Dynamics::handleRaceCarCollision(EntityManager &,
                                     EntityID,
                                     PhysicalProperties &,
                                     WorldP3D &) {

    }

    void
    Dynamics::handleVehicleCollision(EntityManager &,
                                     EntityID,
                                     PhysicalProperties &,
                                     WorldP3D &) {

    }
}
