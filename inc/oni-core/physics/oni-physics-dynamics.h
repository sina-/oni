#pragma once

#include <functional>
#include <memory>

#include <oni-core/entities/oni-entities-client-data-manager.h>
#include <oni-core/component/oni-component-physics.h>
#include <oni-core/component/oni-component-geometry.h>

class b2World;

class b2Body;

namespace oni {
    namespace entities {
        class EntityManager;

        class ClientDataManager;
    }

    namespace math {
        class Rand;
    }

    namespace component {
        struct BrushTrail;
    }

    namespace physics {
        class Projectile;

        class CollisionHandler;

        class Dynamics {
        public:
            Dynamics();

            ~Dynamics();

            void
            tick(entities::EntityManager &,
                 entities::ClientDataManager *clientData,
                 common::r64 tickTime);

            // TODO: Part of the mess that serperation of server and client managers causes is that I can't
            // just call this function from tick()
            static void
            updatePosition(const entities::EntityManager &server,
                           entities::EntityManager &client,
                           common::r64 tickTime);

        public:
            // TODO: Not very happy about this exposure, but it is really the simplest solution right now and only
            // EntityManager uses it for creating components in b2World when creating entities.
            b2World *
            getPhysicsWorld();

        private:
            // TODO: I hate this type of handlers and how I have to register them with bind and shit. Maybe try lambdas?
            void
            handleRocketCollision(entities::EntityManager &,
                                  common::EntityID,
                                  component::PhysicalProperties &,
                                  component::WorldP3D &pos);

            void
            handleVehicleCollision(entities::EntityManager &,
                                   common::EntityID,
                                   component::PhysicalProperties &,
                                   component::WorldP3D &);

            void
            handleRaceCarCollision(entities::EntityManager &,
                                   common::EntityID,
                                   component::PhysicalProperties &,
                                   component::WorldP3D &);

            void
            handleCollision(entities::EntityManager &,
                            common::EntityID,
                            component::PhysicalProperties &,
                            component::WorldP3D &);

            static bool
            isColliding(b2Body *);

        private:
            static void
            updateAge(entities::EntityManager &,
                      common::r64 tickTime);

            static bool
            updateBrush(component::BrushTrail &,
                        common::r32 curMass,
                        common::r32 curDrag,
                        common::r32 x,
                        common::r32 y);

            static void
            addBrushSegment(component::BrushTrail &);


        private:
            std::unique_ptr<b2World> mPhysicsWorld{};
            std::unique_ptr<Projectile> mProjectile{};
            std::unique_ptr<CollisionHandler> mCollisionHandler{};

            std::map<
                    component::PhysicalCategory,
                    std::function<
                            void(entities::EntityManager &,
                                 common::EntityID,
                                 component::PhysicalProperties &,
                                 component::WorldP3D &
                            )>> mCollisionHandlers{};
            std::unique_ptr<math::Rand> mRand{};
        };
    }
}