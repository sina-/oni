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
    Physics::updateJetForce(EntityManager &manager,
                            r64 dt) {
        assert(manager.getSimMode() == SimMode::SERVER);

        auto emptyFuel = std::vector<EntityID>();
        auto view = manager.createView<
                JetForce,
                Orientation,
                PhysicalBody>();
        for (auto &&id: view) {
            auto &body = view.get<PhysicalBody>(id);
            auto &ornt = view.get<Orientation>(id);
            auto &jet = view.get<JetForce>(id);
            if (!subAndZeroClip(jet.fuze, r32(dt))) {
                body.value->ApplyForceToCenter(
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
    Physics::updatePhysWorld(r64 dt) {
        // TODO: entity registry has pointers to mPhysicsWorld internal data structures :(
        // One way to hide it is to provide a function in physics library that creates physical entities
        // for a given entity id an maintains an internal mapping between them without leaking the
        // implementation to outside.
        mPhysicsWorld->Step(dt, 6, 2);
    }

    void
    Physics::updateCarCollision(EntityManager &manager,
                                r64 dt) {
        assert(manager.getSimMode() == SimMode::SERVER);

        auto view = manager.createView<
                Car,
                Orientation,
                WorldP3D,
                WorldP3D_History,
                PhysicalBody>();
        for (auto &&id: view) {
            auto &car = view.get<Car>(id);
            auto &pos = view.get<WorldP3D>(id);
            auto &ornt = view.get<Orientation>(id);
            auto &body = view.get<PhysicalBody>(id);
            auto &input = manager.get<CarInput>(id);
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
                body.value->ApplyForceToCenter(
                        b2Vec2(static_cast<r32>(std::cos(ornt.value) * 30 *
                                                input.throttle),
                               static_cast<r32>(std::sin(ornt.value) * 30 *
                                                input.throttle)),
                        true);
                car.isColliding = false;
            } else {
                auto c = isColliding(body.value);
                if (c.colliding) {
                    car.velocity = vec2{body.value->GetLinearVelocity().x,
                                        body.value->GetLinearVelocity().y};
                    car.angularVelocity = body.value->GetAngularVelocity();
                    pos.x = body.value->GetPosition().x;
                    pos.y = body.value->GetPosition().y;
                    ornt.value = body.value->GetAngle();
                    car.isColliding = true;
                } else {
                    body.value->SetLinearVelocity(b2Vec2{car.velocity.x, car.velocity.y});
                    body.value->SetAngularVelocity(static_cast<float32>(car.angularVelocity));
                    body.value->SetTransform(b2Vec2{pos.x, pos.y},
                                             static_cast<float32>(ornt.value));
                    auto &hist = view.get<WorldP3D_History>(id);
                    hist.add(pos);
                }
            }
        }
    }

    b2World *
    Physics::getPhysicsWorld() {
        return mPhysicsWorld.get();
    }

    Physics::CollisionResult
    Physics::isColliding(b2Body *body) {
        auto result = Physics::CollisionResult{};
        result.colliding = false;
        for (auto *ce = body->GetContactList(); ce; ce = ce->next) {
            auto *c = ce->contact;
            if (c->IsTouching()) {
                auto a = body->GetEntityID();
                auto b = ce->other->GetEntityID();
                assert(a != b);
                b2WorldManifold wm;
                c->GetWorldManifold(&wm);

                result.colliding = true;
                result.pair.a = a;
                result.pair.b = b;

                result.pos.x = wm.points[0].x;
                result.pos.y = wm.points[0].y;
                if (c->GetManifold()->pointCount > 1) {
                    result.pos.x = (result.pos.x + wm.points[1].x) / 2.f;
                    result.pos.y = (result.pos.y + wm.points[1].y) / 2.f;
                }
                return result;
            }
        }
        return result;
    }

    void
    Physics::updateCollision(EntityManager &manager,
                             r64 dt) {
        assert(manager.getSimMode() == SimMode::SERVER);

        auto uniqueCollisions = std::unordered_set<EntityPair, EntityPairHasher>();
        {
            auto view = manager.createView<
                    WorldP3D,
                    PhysicalBody>();
            for (auto &&id: view) {
                auto &body = view.get<PhysicalBody>(id);
                auto result = isColliding(body.value);
                if (result.colliding && uniqueCollisions.find(result.pair) == uniqueCollisions.end()) {
                    uniqueCollisions.emplace(result.pair);

                    auto &propsA = manager.get<PhysicalProperties>(result.pair.a);
                    auto &propsB = manager.get<PhysicalProperties>(result.pair.b);
                    auto pcPair = PhysicalCatPair{propsA.physicalCategory, propsB.physicalCategory};

                    manager.enqueueEvent<Event_Collision>(result.pos, result.pair, pcPair);
                }
            }
        }
    }
}
