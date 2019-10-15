#pragma once

#include <oni-core/component/oni-component-visual.h>
#include <oni-core/entities/parser/oni-entities-parser.h>

namespace oni {
    template<>
    void
    parseAndCreate<ParticleEmitter>(EntityManager &em,
                                    nlohmann::json &json,
                                    std::string_view name);
}