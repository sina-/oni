#include <oni-core/physics/oni-physics.h>

#include <Box2D/Box2D.h>

#include <oni-core/common/oni-common-const.h>
#include <oni-core/math/oni-math-rand.h>
#include <oni-core/game/oni-game-event.h>
#include <oni-core/entities/oni-entities-manager.h>


namespace oni {
    u64
    CollisionHasher::operator()(const Collision &c) const noexcept {
        static auto stdHash = std::hash<u64>();
        if (c.pair.a < c.pair.b) {
            return stdHash(pack_u32(c.pair.a, c.pair.b));
        }
        return stdHash(pack_u32(c.pair.b, c.pair.a));
    }

    bool
    CollisionEqual::operator()(const Collision &rhs,
                               const Collision &lhs) const noexcept {
        auto equalA = rhs.pair.a == lhs.pair.a && rhs.pair.b == lhs.pair.b;
        auto equalB = rhs.pair.a == lhs.pair.b && rhs.pair.b == lhs.pair.a;
        return equalA || equalB;
    }

    class CollisionListener : public b2ContactListener {
    public:
        void
        BeginContact(b2Contact *c) override {
            return;
            auto *fixtureA = c->GetFixtureA();
            auto *fixtureB = c->GetFixtureB();
            auto a = fixtureA->GetBody()->GetEntityID();
            auto b = fixtureB->GetBody()->GetEntityID();

            void *ud = fixtureA->GetBody()->GetUserData();
            if (ud) {
                auto *em = static_cast<EntityManager *>(ud);
                auto *cs = em->mPhysics->mCollisionState.get();
                auto collision = Collision{};
                collision.pair = {a, b};
                if (cs->collisions.find(collision) == cs->collisions.end()) {
                    b2WorldManifold wm;
                    c->GetWorldManifold(&wm);
                    collision.pos.x = wm.points[0].x;
                    collision.pos.y = wm.points[0].y;
                    if (c->GetManifold()->pointCount > 1) {
                        collision.pos.x = (collision.pos.x + wm.points[1].x) / 2.f;
                        collision.pos.y = (collision.pos.y + wm.points[1].y) / 2.f;
                    }
                    cs->collisions.emplace(collision);
                }
            }
        }

        void
        EndContact(b2Contact *contact) override {
            return;
            auto *fixtureA = contact->GetFixtureA();
            auto *fixtureB = contact->GetFixtureB();
            auto a = fixtureA->GetBody()->GetEntityID();
            auto b = fixtureB->GetBody()->GetEntityID();

            void *ud = fixtureA->GetBody()->GetUserData();
            if (ud) {
                auto *em = static_cast<EntityManager *>(ud);
                auto *cs = em->mPhysics->mCollisionState.get();
                auto collision = Collision{};
                collision.pair = {a, b};
                auto p = cs->collisions.find(collision);
                if (p != cs->collisions.end()) {
                    cs->collisions.erase(p);
                } else {
                    assert(false);
                    /*
                    auto pp = cs->collisions->find(collision);
                    if (pp != cs->collisions->end()) {
                    // TODO: This happens if a collision starts and ends in one tick of box2d. For now
                    // I don't do anything and just ignore it, but maybe I should tag it and handle
                    // it in a special way?
                    cs->collisions->erase(pp);
                    } else {
                        assert(false);
                    }
                    */
                }
            }
        }

        void
        PreSolve(b2Contact *contact,
                 const b2Manifold *oldManifold) override {
        }

        void
        PostSolve(b2Contact *c,
                  const b2ContactImpulse *impulse) override {

            auto *fixtureA = c->GetFixtureA();
            auto *fixtureB = c->GetFixtureB();
            auto a = fixtureA->GetBody()->GetEntityID();
            auto b = fixtureB->GetBody()->GetEntityID();

            void *ud = fixtureA->GetBody()->GetUserData();
            if (ud) {
                auto *em = static_cast<EntityManager *>(ud);
                auto *cs = em->mPhysics->mCollisionState.get();
                auto collision = Collision{};
                collision.pair = {a, b};
                if (cs->collisions.find(collision) == cs->collisions.end()) {
                    Physics::_printCollisionDetail(c, impulse);
                    b2WorldManifold wm;
                    c->GetWorldManifold(&wm);
                    collision.pos.x = wm.points[0].x;
                    collision.pos.y = wm.points[0].y;
                    if (c->GetManifold()->pointCount > 1) {
                        collision.pos.x = (collision.pos.x + wm.points[1].x) / 2.f;
                        collision.pos.y = (collision.pos.y + wm.points[1].y) / 2.f;
                    }
                    cs->collisions.emplace(collision);
                }
            }
        }
    };

    Physics::Physics() {
        auto gravity = b2Vec2(0.0f, 0.0f);

        mCollisionState = std::make_unique<CollisionState>();

        mCollisionListener = std::make_unique<CollisionListener>();
        mPhysicsWorld = std::make_unique<b2World>(gravity);
        mPhysicsWorld->SetContactListener(mCollisionListener.get());

        mRand = std::make_unique<Rand>(0, 0);

        // TODO: Can't get this working, its unreliable, when there are lot of collisions in the world, it keeps
        // skipping some of them!
        // mPhysicsWorld->SetContactListener(mCollisionHandler.get());
    }

    Physics::~Physics() = default;

    void
    Physics::updatePhysWorld(EntityManager &em,
                             r64 dt) {
        // TODO: entity registry has pointers to mPhysicsWorld internal data structures :(
        // One way to hide it is to provide a function in physics library that creates physical entities
        // for a given entity id an maintains an internal mapping between them without leaking the
        // implementation to outside.
        mPhysicsWorld->Step(dt, 6, 2);
        _handleCollisions(em);
    }

    void
    Physics::_handleCollisions(EntityManager &em) {
        for (auto it = mCollisionState->collisions.begin(); it != mCollisionState->collisions.end();) {
            auto &propsA = em.get<PhysicalProperties>(it->pair.a);
            auto &propsB = em.get<PhysicalProperties>(it->pair.b);
            auto pcPair = PhysicalCatPair{propsA.physicalCategory, propsB.physicalCategory};

            em.enqueueEvent<Event_Collision>(it->pos, it->pair, pcPair);
            it = mCollisionState->collisions.erase(it);
        }
    }

    void
    Physics::_printCollisionDetail(const b2Contact *contact,
                                   const b2ContactImpulse *impulse) {
        printf("Entities in this collision: \n");
        auto *bodyA = contact->GetFixtureA()->GetBody();
        EntityManager::printEntityType(bodyA);

        auto *bodyB = contact->GetFixtureB()->GetBody();
        EntityManager::printEntityType(bodyB);

        for (u8 i = 0; i < impulse->count; ++i) {
            printf("%f\n", impulse->normalImpulses[i]);
        }
        printf("-------------------\n");
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
