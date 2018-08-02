#pragma once

#include <entt/entity/registry.hpp>

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace entities {

        std::string serialize(entt::DefaultRegistry &source);

        void deserialize(entt::DefaultRegistry &destination, const std::string &data);
    }
}
