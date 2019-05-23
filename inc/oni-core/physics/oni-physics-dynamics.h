#pragma once

#include <functional>
#include <memory>

#include <oni-core/entities/oni-entities-client-data-manager.h>
#include <oni-core/component/oni-component-physic.h>
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

    namespace physics {
        class Projectile;

        class CollisionHandler;

        class Dynamics {

        public:
            explicit Dynamics(common::r32 tickFreq);

            ~Dynamics();

            // TODO: Ideally I shouldn't expose this dude!
            // TODO: When refactoring create-entity stuff, I can instead just expose a member function that handles
            // the dynamics related component creation. But there is a still a bit of an issue. Entities are now saved
            // in two containers, in entt::Registry and b2World. How about creating one class that wraps entt and
            // b2World? At least then it would be easier to handle creation and deletion of entities from single
            // interface without risking leaving around dangling entities that only exist in single container.
            // It's a lot of work and I would need to wrap lot of other data structures with my code to hide Box2D.
            // I could also just make sure the one function that deletes stuff also gets a reference to b2World and
            // make sure things are deleted. So the bigger decision is to make sure all that refactoring is worth
            // the cost really.
            b2World *
            getPhysicsWorld();

            void
            tick(entities::EntityManager &,
                 entities::ClientDataManager *clientData,
                 common::r64 tickTime);

        private:
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

            bool
            isColliding(b2Body *);

        private:
            void
            updateAge(entities::EntityManager &,
                      common::r64 tickTime);

            void
            updatePlacement(entities::EntityManager &,
                            common::r64 tickTime);


        private:
            std::unique_ptr<b2World> mPhysicsWorld{};
            std::unique_ptr<Projectile> mProjectile{};
            std::unique_ptr<CollisionHandler> mCollisionHandler{};

            std::map<component::PhysicalCategory,
                    std::function<void(entities::EntityManager &,
                                       common::EntityID,
                                       component::PhysicalProperties &,
                                       component::WorldP3D &
                    )>> mCollisionHandlers{};
            common::r32 mTickFrequency{};

            std::unique_ptr<math::Rand> mRand{};
        };
    }
}