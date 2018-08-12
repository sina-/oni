#pragma once

#include <entt/entity/registry.hpp>

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace io {
        class Input;
    }

    namespace entities{
        class EntityManager;
    }

    namespace physics {
        class Physic {
        protected:
            virtual ~Physic() = default;

        public:
            virtual void tick(entities::EntityManager &manager, const io::Input &input, common::real32 tickTime) = 0;
        };
    }
}