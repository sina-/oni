#pragma once

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

    class Dynamics {
    public:
        Dynamics();

        ~Dynamics();

        static void
        processInput(EntityManager &,
                     ClientDataManager &,
                     std::unordered_map<EntityID, CarInput> &result);

        static void
        updateCars(EntityManager &,
                   UserInputMap &input,
                   r64 tickTime);

        static void
        updateJetForce(EntityManager &,
                       r64 tickTime);

        void
        updatePhysWorld(r64 tickTime);

        static void
        updateCarCollision(EntityManager &,
                           UserInputMap &input,
                           r64 tickTime);

        void
        updateCollision(EntityManager &,
                        r64 tickTime);

        static void
        syncPosWithPhysWorld(EntityManager &);

        static void
        updateAge(EntityManager &,
                  r64 tickTime);

        static
        void
        updateResting(EntityManager &);

        static void
        updateFadeWithAge(EntityManager &,
                          r64 tickTime);

    public:
        // TODO: Not very happy about this exposure, but it is really the simplest solution right now and only
        // EntityManager uses it for creating components in b2World when creating entities.
        b2World *
        getPhysicsWorld();

    private:
        // TODO: I hate this type of handlers and how I have to register them with bind and shit. Maybe try lambdas?
        void
        handleRocketCollision(EntityManager &,
                              EntityID,
                              PhysicalProperties &,
                              WorldP3D &pos);

        void
        handleVehicleCollision(EntityManager &,
                               EntityID,
                               PhysicalProperties &,
                               WorldP3D &);

        void
        handleRaceCarCollision(EntityManager &,
                               EntityID,
                               PhysicalProperties &,
                               WorldP3D &);

        void
        handleCollision(EntityManager &,
                        EntityID,
                        PhysicalProperties &,
                        WorldP3D &);

        static bool
        isColliding(b2Body *);

    private:

    private:
        std::unique_ptr<b2World> mPhysicsWorld{};
        //std::unique_ptr<CollisionHandler> mCollisionHandler{};

        std::map<
                PhysicalCategory,
                std::function<
                        void(EntityManager &,
                             EntityID,
                             PhysicalProperties &,
                             WorldP3D &
                        )>> mCollisionHandlers{};
        std::unique_ptr<Rand> mRand{};
    };
}