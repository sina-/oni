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

namespace oni {
    struct Collision {
        EntityPair pair{};
        WorldP3D pos{};
        bool handled{false};
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

        // TODO: I now have struct PhysicalBody
        static CollisionResult
        isColliding(b2Body *);

        static void
        _printCollisionDetail(const b2Contact *,
                              const b2ContactImpulse *);

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

    private:
        std::unique_ptr<b2World> mPhysicsWorld{};
        std::unique_ptr<CollisionListener> mCollisionListener{};
        std::unique_ptr<CollisionState> mCollisionState{};
        std::unique_ptr<Rand> mRand{};
    };
}