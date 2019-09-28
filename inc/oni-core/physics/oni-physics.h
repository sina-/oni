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

class b2World;

class b2Body;

namespace oni {
    using UserInputMap = std::unordered_map<EntityID, CarInput>;

    class Physics {
    public:
        Physics();

        ~Physics();

        static void
        updateJetForce(EntityManager &,
                       r64 dt);

        void
        updatePhysWorld(r64 dt);

        static void
        updateCarCollision(EntityManager &,
                           r64 dt);

        static void
        updateCollision(EntityManager &,
                        r64 dt);

    public:
        // TODO: Not very happy about this exposure, but it is really the simplest solution right now and only
        // EntityManager uses it for creating components in b2World when creating entities.
        b2World *
        getPhysicsWorld();

    private:
        struct CollisionResult {
            bool colliding{false};
            EntityPair pair{};
            WorldP3D pos{};
            // Impulse?
        };

    private:
        static CollisionResult
        isColliding(b2Body *);

    private:
        std::unique_ptr<b2World> mPhysicsWorld{};
        std::unique_ptr<Rand> mRand{};
    };
}