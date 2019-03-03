#pragma once

#include <oni-core/common/typedefs.h>
#include <oni-core/component/geometry.h>
#include <oni-core/component/gameplay.h>

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
            explicit Projectile(b2World *);

            ~Projectile();

            void tick(entities::EntityFactory &, entities::ClientDataManager &, common::real64 tickTime);

        private:
            void fireBullet(entities::EntityFactory &entityFactory,
                            const common::real32 bulletSpeed,
                            const component::Placement &,
                            const component::CarConfig &,
                            const common::CarSimDouble &heading,
                            const component::EntityAttachment&
                            );

        private:
            b2World *mPhysicsWorld{};

            const common::real32 mGunCoolDownS;
        };
    }
}
