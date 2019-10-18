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

        auto document = rapidjson::Document();
        if (document.Parse(jsonString.c_str()).HasParseError()) {
            assert(false);
            return EntityManager::nullEntity();
        }

        if (document.IsObject()) {
            auto components = document.FindMember("components");
            if (components != document.MemberEnd()) {
                for (auto component = components->value.Begin(); component != components->value.End(); ++component) {
                    if (component->IsObject()) {
                        rapidjson::StringBuffer compStringBuff;
                        rapidjson::Writer writer(compStringBuff);
                        component->Accept(writer);

                        auto compoName = component->MemberBegin()->name.GetString();
                        printf("Creating component: %s\n", compoName);
                        auto hash = entt::hashed_string(compoName);
                        auto factory = mComponentFactory.find(hash);
                        if (factory != mComponentFactory.end()) {
                            // TODO: so many stream and conversions, can't I just use the stream I get from rapidjson?
                            std::stringstream ss;
                            ss.str(compStringBuff.GetString());
                            cereal::JSONInputArchive reader(ss);
                            // TODO: The issue with this right now is that if something is wrong in json I just get
                            // an assert! What would happen in publish?
                            factory->second(manager, id, reader);
                        } else {
                            assert(false);
                        }
                    }
                }
            }
        } else {
            assert(false);
        }
        return id;
    }
}