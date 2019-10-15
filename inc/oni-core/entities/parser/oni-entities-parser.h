#pragma once

#include <nlohmann/json_fwd.hpp>
#include <oni-core/entities/oni-entities-fwd.h>

namespace oni {
    template<class Component>
    static void
    parseAndCreate(EntityManager &,
                   nlohmann::json &,
                   std::string_view name);
}