#pragma once

#include <entt/entity/registry.hpp>

namespace oni {
    namespace io {
        class Input;
    }

    namespace physics {
        class Physic {
        protected:
            virtual ~Physic() = default;

        public:
            virtual void tick(entt::DefaultRegistry &registry, const io::Input &input, float tickTime) = 0;
        };
    }
}