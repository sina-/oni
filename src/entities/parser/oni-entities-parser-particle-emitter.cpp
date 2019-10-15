#include <oni-core/entities/parser/oni-enitities-parser-particle-emitter.h>

namespace oni {
    template<>
    void
    parseAndCreate<ParticleEmitter>(EntityManager &em,
                                    nlohmann::json &json,
                                    std::string_view name) {

    }
}