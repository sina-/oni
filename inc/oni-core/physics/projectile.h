#pragma once

#include <oni-core/common/typedefs.h>

class b2World;

namespace oni {
    namespace math {
        struct vec3;
    }
    namespace entities {
        class EntityManager;

        class ClientDataManager;
    }

    namespace physics {
        class Projectile {
        public:
            Projectile(b2World *);

            ~Projectile();

            void tick(entities::EntityManager &, entities::ClientDataManager &, common::real64 tickTime);

        private:
            common::EntityID createBullet(entities::EntityManager &, const math::vec3 &pos);

        private:
            b2World *mPhysicsWorld{};
        };
    }
}
