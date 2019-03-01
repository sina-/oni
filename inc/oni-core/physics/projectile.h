#pragma once

#include <oni-core/common/typedefs.h>
#include <oni-core/component/geometry.h>

class b2World;

namespace oni {
    namespace math {
        struct vec2;
        struct vec3;
    }
    namespace entities {
        class EntityManager;

        class EntityFactory;

        class ClientDataManager;
    }

    namespace physics {
        class Projectile {
        public:
            Projectile(b2World *);

            ~Projectile();

            void tick(entities::EntityManager &, entities::EntityFactory &, entities::ClientDataManager &,
                      common::real64 tickTime);

        private:
            b2World *mPhysicsWorld{};
        };
    }
}
