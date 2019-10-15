#include <oni-core/entities/oni-entities-factory.h>

#include <fstream>
#include <nlohmann/json.hpp>

#include <oni-core/component/oni-component-visual.h>
#include <oni-core/entities/parser/oni-entities-parser-include.h>

namespace oni {
    oni::EntityFactory::EntityFactory(const std::string &) {

    }

    void to_json(nlohmann::json&j, const ParticleEmitter& pe) {

    }

    void from_json(const nlohmann::json& j, ParticleEmitter& pe) {

    }

    EntityID
    oni::EntityFactory::createEntity(EntityManager &em,
                                     EntityName name) {
        std::ifstream i(name);
        auto j = nlohmann::json();
        i >> j;
        auto components = j["components"];
        for (auto &&component: components) {
            if (component.is_object()) {
                for (auto&&[key, value]: component.items()) {
                    // TODO: These two alternatives, both sucks, and they don't fit into the engine
//                   parseAndCreate<ParticleEmitter>(em, value, key);
                    if(key == "ParticleEmitter") {
                        auto pe = value.get<ParticleEmitter>();
                    }
                }
            }
        }

        // * create entity
        // * find the entity in the json
        // * get the list of components from json
        // * create each component using a mapping that says which component each should be
        // NOTE: Can I use some other format that is easier than json to match it to cpp struct? something
        // like protobuff would be nice but not as fucking heavy.
    }
}