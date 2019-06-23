#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-gameplay.h>
#include <oni-core/component/oni-component-physics.h>

class b2World;

namespace oni {
    namespace math {
        struct vec2;
        struct vec3;
    }

    namespace entities {
        class EntityManager;

        class ClientDataManager;
    }

    namespace component {
        struct CarConfig;
    }

    namespace physics {
        class Projectile {
        public:
            explicit Projectile(b2World *);

            ~Projectile();

            void
            tick(entities::EntityManager &,
                 entities::ClientDataManager *,
                 common::r64 tickTime);

        private:
            void
            fireRocket(entities::EntityManager &manager,
                       const oni::component::Car &car,
                       const component::WorldP3D &pos,
                       const component::Heading &heading,
                       const component::CarConfig &carConfig,
                       const component::EntityAttachment &attachments
            );

        private:
            b2World *mPhysicsWorld{};

            const common::r32 mGunCoolDownS;
        };
    }
}
