#pragma once

#include <oni-core/entities/client-data-manager.h>

class b2World;

namespace oni {
    namespace entities {
        class EntityManager;

        class ClientDataManager;
    }

    namespace physics {
        class Dynamics {

        public:
            explicit Dynamics(common::real32 tickFreq);

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
            b2World *getPhysicsWorld();

            void tick(entities::EntityManager &manager,
                      entities::ClientDataManager &clientData,
                      common::real64 tickTime);

        private:
            std::unique_ptr<b2World> mPhysicsWorld{};
            common::real32 mTickFrequency{};
        };
    }
}