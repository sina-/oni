#include <oni-core/entities/oni-entities-factory.h>

// NOTE: For cereal to not throw exceptions and disable stupid noexcept constructor that throws in debug!
#define JSON_NOEXCEPTION

#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include <cereal/archives/json.hpp>

#include <oni-core/component/oni-component-visual.h>
#include <oni-core/entities/oni-entities-manager.h>
#include <oni-core/entities/oni-entities-serialization.h>

// NOTE: Call to make_nvp is important, otherwise order of the values in the json has to match that of struct
#define COMPONENT_FACTORY_DEFINE(factory, COMPONENT_NAME)                           \
{                                                                                   \
        ComponentFactory cf = [](EntityManager &em,                                 \
                                 EntityID id,                                       \
                                 cereal::JSONInputArchive &reader) {                \
            auto &component = em.createComponent<COMPONENT_NAME>(id);               \
            reader(cereal::make_nvp(#COMPONENT_NAME, component));                   \
        };                                                                          \
        factory->registerComponentFactory({#COMPONENT_NAME}, std::move(cf));        \
}

namespace oni {
    oni::EntityFactory::EntityFactory(FilePath fp) : mEntityResourcePath(std::move(fp)) {
        COMPONENT_FACTORY_DEFINE(this, WorldP3D)
        COMPONENT_FACTORY_DEFINE(this, WorldP2D)
        COMPONENT_FACTORY_DEFINE(this, ParticleEmitter)
    }

    void
    EntityFactory::registerComponentFactory(const Component_Name &name,
                                            ComponentFactory &&cb) {
        assert(mComponentFactory.find(name.value) == mComponentFactory.end());
        mComponentFactory.emplace(name.value, std::move(cb));
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
    oni::EntityFactory::createEntity(EntityManager &manager,
                                     const EntityType_Name &name) {
        auto entityType = getEntityType(name);

        auto fp = getEntityResourcePath(name);
        if (fp.value.empty()) {
            return EntityManager::nullEntity();
        }

        // TODO: Pre-load all the jsons
        std::ifstream jsonStream(fp.value);
        if (!jsonStream.is_open()) {
            assert(false);
            return EntityManager::nullEntity();
        }

        // TODO: Check for valid entityType?
        // TODO: After having registry take care of entity name string to entity type lookup I can
        // just add an overload to createEntity that receives EntityType_Name and returns and ID or nullEntity
        // if mapping is missing! Solves so many issues.
        auto id = manager.createEntity(entityType);

        auto jsonString = std::string((std::istreambuf_iterator<char>(jsonStream)), std::istreambuf_iterator<char>());
        //nlohmann::json root;
        //jsonStream >> root;
        auto document = rapidjson::Document();
        if (document.Parse(jsonString.c_str()).HasParseError()) {
            assert(false);
            return EntityManager::nullEntity();
        }

        //auto root = nlohmann::json::parse(jsonString);
        if (document.IsObject()) {
            auto components = document.FindMember("components");
            if (components != document.MemberEnd()) {
                for (auto component = components->value.MemberBegin();
                     component != components->value.MemberEnd(); ++component) {
                    //auto components = root["components"];
                    if (component->value.IsObject()) {
                        auto buffer = rapidjson::StringBuffer{};
                        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                        component->value.Accept(writer);
                        auto value = buffer.GetString();

                        auto key = component->value.FindMember("dummy")->value.MemberBegin()->name.GetString();
                        auto hash = entt::hashed_string(key);
                        {
                            std::ostringstream ss;
                            {
                                cereal::JSONOutputArchive oa{ss};
                                auto pe = ParticleEmitter{};
                                oa(CEREAL_NVP(pe));
                            }
                            auto result = ss.str();
                            printf("example:\n %s\n", result.c_str());
                        }
                        printf("value:\n %s\n", value);
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
                        auto factory = mComponentFactory.find(hash);
                        if (factory != mComponentFactory.end()) {
                            // TODO: so many stream and conversions, can't I just use the stream I get from rapidjson?
                            std::stringstream ss;
                            ss.str(value);
                            cereal::JSONInputArchive reader(ss);
                            // TODO: The issue with this right now is that if something is wrong in json I just get
                            // an assert! What would happen in publish?
                            factory->second(manager, id, reader);
                        } else {
                            assert(false);
                        }

                        //const auto & type = parseType<InvalidComponent>({key});

                        // 4 polymorphism
                        //createComponent({key}, value);

                    }
                }
            }
        }
        return id;
    }

    // * create entity
    // * find the entity in the json
    // * get the list of components from json
    // * create each component using a mapping that says which component each should be
    // NOTE: Can I use some other format that is easier than json to match it to cpp struct? something
    // like protobuff would be nice but not as fucking heavy.
}