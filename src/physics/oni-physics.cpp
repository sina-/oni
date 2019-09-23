#include <oni-core/physics/oni-physics.h>

#include <unordered_set>

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

    Physics::Physics() {
        auto gravity = b2Vec2(0.0f, 0.0f);
        ///mCollisionHandler = std::make_unique<CollisionHandler>();
        mPhysicsWorld = std::make_unique<b2World>(gravity);
        mRand = std::make_unique<Rand>(0, 0);

        // TODO: Can't get this working, its unreliable, when there are lot of collisions in the world, it keeps
        // skipping some of them!
        // mPhysicsWorld->SetContactListener(mCollisionHandler.get());
    }

    Physics::~Physics() = default;

    void
    Physics::processInput(EntityManager &manager,
                          ClientDataManager &clientData,
                          std::unordered_map<EntityID, CarInput> &result) {
        assert(manager.getSimMode() == SimMode::SERVER);

        auto carView = manager.createView<
                Orientation,
                Car,
                CarConfig>();
        for (auto &&id: carView) {
            auto input = clientData.getClientInput(id);
            auto &car = carView.get<Car>(id);
            auto &ornt = carView.get<Orientation>(id);
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
                car.velocity = car.velocity + vec2{static_cast<r32>(cos(ornt.value)),
                                                   static_cast<r32>(sin(ornt.value))};
            }
            if (input->isPressed(GLFW_KEY_SPACE)) {
                result[id].eBrake = 1.f;
            }
        }
    }

    void
    Physics::updateCars(EntityManager &manager,
                        UserInputMap &input,
                        r64 tickTime) {
        assert(manager.getSimMode() == SimMode::SERVER);

        /// Update car
        {
            auto carView = manager.createView<
                    WorldP3D,
                    Orientation,
                    Scale,
                    Car,
                    CarConfig>();
            for (auto &&id: carView) {
                auto &car = carView.get<Car>(id);
                auto &ornt = carView.get<Orientation>(id);
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
                const auto oldOrnt = ornt;
                tickCar(car, carPos, ornt, carConfig, input[id], tickTime);

                // TODO: This is probably not needed, client should be able to figure this out.
                auto velocity = car.velocityLocal.len();
                auto distanceFromCamera = 1 + velocity * 2 / car.maxVelocityAbsolute;
                if (!almost_Equal(oldPos.x, carPos.x) ||
                    !almost_Equal(oldPos.y, carPos.y) ||
                    !almost_Equal(oldPos.y, carPos.y) ||
                    !almost_Equal(oldOrnt.value, ornt.value)) {

                    car.distanceFromCamera = distanceFromCamera;

                    manager.markForNetSync(id);
                }
            }
        }

        /// Update tires
        // TODO: This is useless in Engine code. Input processing should be able to take care of this.
        {
            auto view = manager.createView<
                    EntityAttachment,
                    Car>();
            for (auto &&id : view) {
                const auto &attachments = view.get<EntityAttachment>(id);
                const auto &car = view.get<Car>(id);
                for (size i = 0; i < attachments.entities.size(); ++i) {
                    if (attachments.entityTypes[i] == EntityType::VEHICLE_TIRE_FRONT) {
                        auto &ornt = manager.get<Orientation>(attachments.entities[i]).value;

                        // TODO: I shouldn't need to do this kinda of rotation transformation, x-1.0f + 90.0f.
                        // There seems to be something wrong with the way tires are created in the beginning
                        ornt = static_cast<r32>(car.steerAngle +
                                                toRadians(90.0f));

                        manager.markForNetSync(attachments.entities[i]);
                    }
                }
            }
        }
    }

    void
    Physics::updateJetForce(EntityManager &manager,
                            r64 tickTime) {
        assert(manager.getSimMode() == SimMode::SERVER);

        auto emptyFuel = std::vector<EntityID>();
        auto view = manager.createView<
                JetForce,
                Orientation,
                PhysicalProperties>();
        for (auto &&id: view) {
            auto *body = manager.getEntityBody(id);
            auto &ornt = view.get<Orientation>(id);
            auto &jet = view.get<JetForce>(id);
            if (!subAndZeroClip(jet.fuze, r32(tickTime))) {
                body->ApplyForceToCenter(
                        b2Vec2(cos(ornt.value) * jet.force,
                               sin(ornt.value) * jet.force),
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
    Physics::updatePhysWorld(r64 tickTime) {
        // TODO: entity registry has pointers to mPhysicsWorld internal data structures :(
        // One way to hide it is to provide a function in physics library that creates physical entities
        // for a given entity id an maintains an internal mapping between them without leaking the
        // implementation to outside.
        mPhysicsWorld->Step(tickTime, 6, 2);
    }

    void
    Physics::updateCarCollision(EntityManager &manager,
                                UserInputMap &input,
                                r64 tickTime) {
        assert(manager.getSimMode() == SimMode::SERVER);

        auto view = manager.createView<
                Car,
                Orientation,
                WorldP3D,
                WorldP3D_History,
                PhysicalProperties>();
        for (auto &&id: view) {
            auto &props = view.get<PhysicalProperties>(id);
            auto &car = view.get<Car>(id);
            auto &pos = view.get<WorldP3D>(id);
            auto &ornt = view.get<Orientation>(id);
            auto *body = manager.getEntityBody(id);
            auto collisionPairs = std::unordered_set<EntityPair, EntityPairHasher>();
            // NOTE: If the car was in collision previous tick, that is what isColliding is tracking,
            // just apply user input to box2d representation of physical body without syncing
            // car dynamics with box2d physics, that way the next tick if the
            // car was ornt out of collision it will start sliding out and things will run smoothly according
            // to car dynamics calculation. If the car is still ornt against other objects, it will be
            // stuck as it was and I will skip dynamics and just sync it to  position and orientation
            // from box2d. This greatly improves game feeling when there are collisions and
            // solves lot of stickiness issues.
            if (car.isColliding) {
                // TODO: Right now 30 is just an arbitrary multiplier, maybe it should be based on some value in
                // carconfig?
                // TODO: Test other type of forces if there is a combination of acceleration and steering to sides
                body->ApplyForceToCenter(
                        b2Vec2(static_cast<r32>(std::cos(ornt.value) * 30 *
                                                input[id].throttle),
                               static_cast<r32>(std::sin(ornt.value) * 30 *
                                                input[id].throttle)),
                        true);
                car.isColliding = false;
            } else {
                if (isColliding(body, collisionPairs)) {
                    car.velocity = vec2{body->GetLinearVelocity().x,
                                        body->GetLinearVelocity().y};
                    car.angularVelocity = body->GetAngularVelocity();
                    pos.x = body->GetPosition().x;
                    pos.y = body->GetPosition().y;
                    ornt.value = body->GetAngle();
                    car.isColliding = true;
                } else {
                    body->SetLinearVelocity(b2Vec2{car.velocity.x, car.velocity.y});
                    body->SetAngularVelocity(static_cast<float32>(car.angularVelocity));
                    body->SetTransform(b2Vec2{pos.x, pos.y},
                                       static_cast<float32>(ornt.value));
                    auto &hist = view.get<WorldP3D_History>(id);
                    hist.add(pos);
                }
            }
        }
    }

    void
    Physics::syncPosWithPhysWorld(EntityManager &manager) {
        assert(manager.getSimMode() == SimMode::SERVER ||
               manager.getSimMode() == SimMode::CLIENT);

        auto view = manager.createView<
                WorldP3D,
                Orientation,
                Scale,
                PhysicalProperties>();

        for (auto &&id: view) {
            auto *body = manager.getEntityBody(id);
            auto &bPos = body->GetPosition();
            auto &ePos = view.get<WorldP3D>(id);
            auto &ornt = view.get<Orientation>(id);
            auto &scale = view.get<Scale>(id);

            if (!almost_Equal(ePos.x, bPos.x) ||
                !almost_Equal(ePos.y, bPos.y) ||
                !almost_Equal(ornt.value, body->GetAngle())) {

                ePos.x = bPos.x;
                ePos.y = bPos.y;

                if (manager.has<WorldP3D_History>(id)) {
                    manager.get<WorldP3D_History>(id).add(ePos);
                }

                ornt.value = body->GetAngle();
                manager.markForNetSync(id);
            }
        }
    }

    b2World *
    Physics::getPhysicsWorld() {
        return mPhysicsWorld.get();
    }

    bool
    Physics::isColliding(b2Body *body,
                         std::unordered_set<EntityPair, EntityPairHasher> &result) {
        for (auto *ce = body->GetContactList(); ce; ce = ce->next) {
            auto *c = ce->contact;
            if (c->IsTouching()) {
                auto a = body->GetEntityID();
                auto b = ce->other->GetEntityID();
                assert(a != b);
                result.insert({a, b});
                return true;
            }
        }
        return false;
    }

    void
    Physics::updateAge(EntityManager &manager,
                       r64 tickTime) {
        assert(manager.getSimMode() == SimMode::CLIENT ||
               manager.getSimMode() == SimMode::SERVER);

        auto view = manager.createView<
                TimeToLive>();
        for (auto &&id: view) {
            auto &age = view.get<TimeToLive>(id);
            age.currentAge += tickTime;
            if (age.currentAge > age.maxAge) {
                if (manager.has<Tag_SplatOnDeath>(id)) {
                    auto &pos = manager.get<WorldP3D>(id);
                    auto &size = manager.get<Scale>(id);
                    auto &tag = manager.get<EntityAssetsPack>(id);
                    auto &ornt = manager.get<Orientation>(id);

                    auto callback = [&manager, id]() {
                        manager.deleteEntity(id);
                    };

                    manager.enqueueEvent<Event_SplatOnDeath>(pos, size, ornt, tag, std::move(callback));
                } else {
                    manager.markForDeletion(id);
                }
            }
        }
        manager.flushDeletions();
    }

    void
    Physics::updateResting(EntityManager &manager) {
        assert(manager.getSimMode() == SimMode::CLIENT);

        auto view = manager.createView<
                PhysicalProperties,
                Scale,
                EntityAssetsPack,
                WorldP3D,
                Orientation,
                Tag_SplatOnRest>();
        for (auto &&id: view) {
            auto *body = manager.getEntityBody(id);
            if (!body->IsAwake()) {
                auto &pos = view.get<WorldP3D>(id);
                auto &size = view.get<Scale>(id);
                auto &tag = view.get<EntityAssetsPack>(id);
                auto &ornt = view.get<Orientation>(id);

                auto callback = [&manager, id]() {
                    manager.deleteEntity(id);
                };
                // TODO: This will create copy for all. Good place for profiling and optimization as these entities
                // are often particles
                manager.enqueueEvent<Event_SplatOnRest>(pos, size, ornt, tag, std::move(callback));
            }
        }
    }

    void
    Physics::updateCollision(EntityManager &manager,
                             r64 tickTime) {
        assert(manager.getSimMode() == SimMode::SERVER);

        auto uniqueCollisions = std::unordered_set<EntityPair, EntityPairHasher>();
        {
            auto view = manager.createView<
                    WorldP3D,
                    PhysicalProperties>();
            for (auto &&id: view) {
                auto *body = manager.getEntityBody(id);
                isColliding(body, uniqueCollisions);
            }
        }

        for (auto &&pair: uniqueCollisions) {
            auto &propsA = manager.get<PhysicalProperties>(pair.a);
            auto &propsB = manager.get<PhysicalProperties>(pair.b);
            // TODO: This is wrong, I need to get the position of point of contact
            auto &pos = manager.get<WorldP3D>(pair.a);
            auto pcPair = PhysicalCatPair{propsA.physicalCategory, propsB.physicalCategory};

            manager.enqueueEvent<Event_Collision>(pos, pair, pcPair);
        }
    }
}
