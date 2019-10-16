#include <oni-core/entities/oni-entities-factory.h>

#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include <utility>

#include <oni-core/component/oni-component-visual.h>
#include <oni-core/entities/oni-entities-manager.h>
#include <oni-core/entities/oni-entities-serialization.h>

namespace oni {
    void
    EntityFactory::registerComponentFactory(const Component_Name &name,
                                            componentFactory &&cb) {
        assert(mComponentFactory.find(name.value) != mComponentFactory.end());
        mComponentFactory.emplace(name.value, std::move(cb));
    }

    oni::EntityFactory::EntityFactory(FilePath fp) : mEntityResourcePath(std::move(fp)) {
    }

    void
    EntityFactory::registerEntityType(EntityType_Name name,
                                      EntityType et) {
        // TODO: Not too sure about char * lifetime of HashedString!
        mEntityNameLookup.emplace(name.value, et);

        auto path = FilePath{};
        path.value.append(mEntityResourcePath.value);
        path.value.append(name.value.data());
        path.value.append(".json");
        mEntityResourcePathLookup.emplace(name.value, std::move(path));
    }

    EntityType
    EntityFactory::getEntityType(const EntityType_Name &name) {
        auto type = mEntityNameLookup.find(name.value);
        if (type != mEntityNameLookup.end()) {
            return type->second;
        }
        assert(false);
        return {};
    }

    const FilePath &
    EntityFactory::getEntityResourcePath(const EntityType_Name &name) {
        auto path = mEntityResourcePathLookup.find(name.value);
        if (path != mEntityResourcePathLookup.end()) {
            return path->second;
        }
        assert(false);
        static auto empty = FilePath{};
        return empty;
    }

    EntityID
    oni::EntityFactory::createEntity(EntityManager &em,
                                     const EntityType_Name &name) {
        auto entityType = getEntityType(name);

        auto fp = getEntityResourcePath(name);
        if (fp.value.empty()) {
            return EntityManager::nullEntity();
        }

        auto id = em.createEntity(entityType);

        // TODO: Pre-load all the jsons
        std::ifstream jsonStream(fp.value);
        auto root = nlohmann::json();
        jsonStream >> root;
        auto components = root["components"];
        for (auto &&component: components) {
            if (component.is_object()) {
                for (auto&&[key, value]: component.items()) {
                    auto hash = entt::hashed_string(key.c_str());
                    // TODO: These two alternatives, both suck, and they don't fit into the engine
                    // 1
//                   parseAndCreate<ParticleEmitter>(em, value, key);
                    // 2
//                    if (key == "ParticleEmitter") {
//                        auto pe = value.get<ParticleEmitter>();
//                        em.createComponent<ParticleEmitter>(id, pe);
                    // 3
                    // create a map of string to function pointers where clients can register functions that
                    // will create the component given a string which is probably better tha a giant
                    // if else statement of string comparisons! since map will do a one hash and then
                    // fast lookup to the exact function. The down side is the stupid register handler
                    // pattern where you can't tell by looking at the caller what will happen, you have to
                    // go to the place where registration happens :/ It would be much nicer if I can just
                    // do a go to definition and immediately see the parser
                    // and to avoid duplicating serialization functions I can use cereal
//                    std::stringstream ss;
//                    ss.str(value);
//                    cereal::JSONInputArchive reader(ss);
//                    mComponentFactory.at(hash)(em, reader);
//
//                    WorldP3D pe;
//                    reader(pe);
//                    em.createComponent<WorldP3D>(0, pe);

                    //const auto & type = parseType<InvalidComponent>({key});

                    // 4 polymorphism
                    //createComponent({key}, value);

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