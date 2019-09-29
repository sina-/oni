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
    Physics::updatePhysWorld(r64 dt) {
        // TODO: entity registry has pointers to mPhysicsWorld internal data structures :(
        // One way to hide it is to provide a function in physics library that creates physical entities
        // for a given entity id an maintains an internal mapping between them without leaking the
        // implementation to outside.
        mPhysicsWorld->Step(dt, 6, 2);
    }

    b2World *
    Physics::getPhysicsWorld() {
        return mPhysicsWorld.get();
    }

    CollisionResult
    Physics::isColliding(b2Body *body) {
        auto result = CollisionResult{};
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
}
