#pragma once

#include <unordered_set>
#include <functional>
#include <memory>

#include <oni-core/entities/oni-entities-client-data-manager.h>
#include <oni-core/entities/oni-entities-fwd.h>
#include <oni-core/component/oni-component-physics.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-fwd.h>
#include <oni-core/math/oni-math-fwd.h>
#include <oni-core/physics/oni-physics-fwd.h>

class b2World;

class b2Body;

class b2ContactImpulse;

class b2WorldManifold;

namespace oni {
    struct Collision {
        EntityPair pair{};
        WorldP3D pos{};
        Impulse2D impulse{};
        bool handled{false};
        EntityManager *em{}; // NOTE: Just for debugging
    };

    struct CollisionHasher {
        oni::size
        operator()(const Collision &) const noexcept;
    };

    struct CollisionEqual {
        bool
        operator()(const Collision &,
                   const Collision &) const noexcept;
    };

    using UserInputMap = std::unordered_set<EntityID, CarInput>;
    using UniqueCollisions = std::unordered_set<Collision, CollisionHasher, CollisionEqual>;

    struct CollisionState {
        UniqueCollisions collisions{};
    };

    // TODO: Merge with Collision
    struct CollisionResult {
        bool colliding{false};
        EntityPair pair{};
        WorldP3D pos{};
        Force2D impulse{};
    };

    class Physics {
    public:
        Physics();

        ~Physics();

        void
        updatePhysWorld(EntityManager &,
                        r64 dt);

        static void
        _printCollisionDetail(const b2Contact *,
                              const Impulse2D &);

        static void
        _printCollisionDetail(const Collision &);

    private:
        friend EntityManager;
        friend CollisionListener;

        // TODO: Not very happy about this exposure, but it is really the simplest solution right now and only
        // EntityManager uses it for creating components in b2World when creating entities.
        b2World *
        getPhysicsWorld();

    private:
        void
        _handleCollisions(EntityManager &);

        void
        _handleCarCollision(EntityManager &,
                            EntityID);

    private:
        std::unique_ptr<b2World> mPhysicsWorld{};
        std::unique_ptr<CollisionListener> mCollisionListener{};
        std::unique_ptr<CollisionState> mCollisionState{};
        std::unique_ptr<Rand> mRand{};
    };
}