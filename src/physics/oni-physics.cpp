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
        }

        void
        EndContact(b2Contact *contact) override {
        }

        void
        PreSolve(b2Contact *contact,
                 const b2Manifold *oldManifold) override {
        }

        void
        PostSolve(b2Contact *contact,
                  const b2ContactImpulse *impulse) override {

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
                if (cs->collisions.find(collision) == cs->collisions.end()) {
                    collision.em = em;
                    auto wm = b2WorldManifold{};
                    contact->GetWorldManifold(&wm);

                    for (u8 i = 0; i < impulse->count; ++i) {
                        collision.impulse.value += impulse->normalImpulses[i];
                    }
                    if (impulse->count) {
                        collision.impulse.value /= impulse->count;
                    }

                    collision.impulse.normal.x = wm.normal.x;
                    collision.impulse.normal.y = wm.normal.y;

                    Physics::_printCollisionDetail(contact, collision.impulse);

                    collision.pos.x = wm.points[0].x;
                    collision.pos.y = wm.points[0].y;
                    if (contact->GetManifold()->pointCount > 1) {
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
            constexpr auto raceCar = PhysicalCategory::GET("RaceCar");
            if (pcPair.a == raceCar) {
                _handleCarCollision(em, it->pair.a);
            }
            if (pcPair.b == raceCar) {
                _handleCarCollision(em, it->pair.b);
            }

            // TODO: Arbitary number!
            if (it->impulse.value >= 0.1f) {
//                Physics::_printCollisionDetail(*it);
                em.enqueueEvent<Event_Collision>(it->pos, it->impulse, it->pair, pcPair);
            }

            it = mCollisionState->collisions.erase(it);
        }
    }

    void
    Physics::_handleCarCollision(EntityManager &em,
                                 EntityID id) {
        auto &car = em.get<Car>(id);
        auto &body = em.get<PhysicalBody>(id);
        auto &ornt = em.get<Orientation>(id);
        auto &pos = em.get<WorldP3D>(id);
        auto &input = em.get<CarInput>(id);
        auto &hist = em.get<WorldP3D_History>(id);
        // NOTE: If the car was in collision previous tick, that is what isColliding is tracking,
        // just apply user input to box2d representation of physical body without syncing
        // car dynamics with box2d physics, that way the next tick if the
        // car was ornt out of collision it will start sliding out and things will run smoothly according
        // to car dynamics calculation. If the car is still ornt against other objects, it will be
        // stuck as it was and I will skip dynamics and just sync it to  position and orientation
        // from box2d. This greatly improves game feeling when there are collisions and
        // solves lot of stickiness issues.
        if (car.isColliding) {
            auto dirX = std::cos(ornt.value);
            auto dirY = 1 - dirX;
            // TODO: Arbitrary multiplier, maybe it should be based on some value in carconfig?
            // TODO: Test other type of forces if there is a combination of acceleration and steering to sides
            body.value->ApplyForceToCenter(b2Vec2(dirX * input.throttle * 1.f, dirY * input.throttle * 1.f), true);
            car.isColliding = false;
        } else {
            car.velocity = vec2{body.value->GetLinearVelocity().x,
                                body.value->GetLinearVelocity().y};
            car.angularVelocity = body.value->GetAngularVelocity();
            pos.x = body.value->GetPosition().x;
            pos.y = body.value->GetPosition().y;
            ornt.value = body.value->GetAngle();
            car.isColliding = true;
        }
    }

    void
    Physics::_printCollisionDetail(const b2Contact *contact,
                                   const Impulse2D &impulse) {
        auto *bodyA = contact->GetFixtureA()->GetBody();
        auto *bodyB = contact->GetFixtureA()->GetBody();
        auto c = Collision{};
        c.pair.a = bodyA->GetEntityID();
        c.pair.b = bodyB->GetEntityID();
        c.em = static_cast<EntityManager *>(bodyA->GetUserData());
        c.impulse = impulse;
        _printCollisionDetail(c);
    }

    void
    Physics::_printCollisionDetail(const Collision &collision) {
        printf("Entities in this collision: \n");
        collision.em->printEntityType(collision.pair.a);
        collision.em->printEntityType(collision.pair.b);

        auto impulse = collision.impulse.normal;
        impulse.multiply(collision.impulse.value);

        printf("Impulse vector: ");
        impulse.print();
        printf("-------------------\n");
    }

    b2World *
    Physics::getPhysicsWorld() {
        return mPhysicsWorld.get();
    }
}
