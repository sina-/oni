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
        class CollisionHandler;

        using UserInputMap = std::unordered_map<common::EntityID, io::CarInput>;

        class Dynamics {
        public:
            Dynamics();

            ~Dynamics();

            static void
            processInput(entities::EntityManager &,
                         entities::ClientDataManager &,
                         std::unordered_map<common::EntityID, io::CarInput> &result);

            static void
            updateCars(entities::EntityManager &,
                       UserInputMap &input,
                       common::r64 tickTime);

            static void
            updateJetForce(entities::EntityManager &,
                           common::r64 tickTime);

            void
            updatePhysWorld(common::r64 tickTime);

            static void
            updateCarCollision(entities::EntityManager &,
                               UserInputMap &input,
                               common::r64 tickTime);

            void
            updateCollision(entities::EntityManager &,
                            common::r64 tickTime);

            static void
            syncPosWithPhysWorld(entities::EntityManager &);

            static void
            updateAge(entities::EntityManager &,
                      common::r64 tickTime);

            static void
            updateCooldDowns(entities::EntityManager &,
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

        private:
            std::unique_ptr<b2World> mPhysicsWorld{};
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