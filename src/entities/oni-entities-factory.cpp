#include <oni-core/entities/oni-entities-factory.h>

// NOTE: For cereal to not throw exceptions and disable stupid noexcept constructor that throws in debug!
#define JSON_NOEXCEPTION

#include <cassert>
#include <fstream>
#include <istream>
#include <cereal/archives/json.hpp>

#include <oni-core/component/oni-component-visual.h>
#include <oni-core/entities/oni-entities-manager.h>
#include <oni-core/util/oni-util-enum.h>
#include <oni-core/entities/oni-entities-serialization.h>

#define COMPONENT_FACTORY_DEFINE(factory, COMPONENT_NAME)                               \
{                                                                                       \
        ComponentFactory cf = [](EntityManager &em,                                     \
                                 EntityID id,                                           \
                                 cereal::JSONInputArchive &reader) {                    \
            auto &component = em.createComponent<COMPONENT_NAME>(id);                   \
            reader(component);                                                          \
        };                                                                              \
        constexpr auto componentName = ComponentName{#COMPONENT_NAME};                  \
        factory->registerComponentFactory(componentName, std::move(cf));                \
}

namespace {
    std::optional<rapidjson::Document>
    _readJson(const oni::EntityDefDirPath &fp) {
        // TODO: Pre-load all the jsons
        std::ifstream jsonStream(fp.path);
        if (!jsonStream.is_open()) {
            return {};
        }

        auto jsonString = std::string((std::istreambuf_iterator<char>(jsonStream)), std::istreambuf_iterator<char>());

        auto document = rapidjson::Document();
        if (document.Parse(jsonString.c_str()).HasParseError()) {
            return {};
        }

        if (!document.IsObject()) {
            return {};
        }

        return document;
    }

    bool
    _createComponent(std::unordered_map<oni::Hash, oni::ComponentFactory> &factoryMap,
                     oni::EntityManager &manager,
                     oni::EntityID id,
                     rapidjson::Document &document,
                     rapidjson::Document::MemberIterator component) {
        auto compoName = component->name.GetString();

        rapidjson::StringBuffer compStringBuff;
        rapidjson::Writer writer(compStringBuff);

        // NOTE: I need to recreate the object because rapidjson component splits up component name
        // and data but cereal expects a json object where the component name is the key for
        // the object in the form: {"Velocity": {"x": 1, "y": 1, "z": 1}}
        auto data = rapidjson::Value(rapidjson::kObjectType);
        data.AddMember(component->name.Move(), component->value.Move(), document.GetAllocator());
        data.Accept(writer);

        auto hash = oni::HashedString::makeHashFromCString(compoName);
        auto factory = factoryMap.find(hash);
        if (factory != factoryMap.end()) {
            // TODO: so many stream and conversions, can't I just use the stream I get from rapidjson?
            std::istringstream ss;
            ss.str(compStringBuff.GetString());
            cereal::JSONInputArchive reader(ss);
            // TODO: The issue with this right now is that if something is wrong in json I just get
            // an assert! What would happen in publish?
            factory->second(manager, id, reader);
            return true;
        } else {
            return false;
        }
    }
}

namespace oni {
    oni::EntityFactory::EntityFactory(EntityDefDirPath &&fp) : mEntityResourcePath(std::move(fp)) {
        COMPONENT_FACTORY_DEFINE(this, WorldP3D)
        COMPONENT_FACTORY_DEFINE(this, WorldP2D)
        COMPONENT_FACTORY_DEFINE(this, Direction)
        COMPONENT_FACTORY_DEFINE(this, Orientation)
        COMPONENT_FACTORY_DEFINE(this, Scale)
        COMPONENT_FACTORY_DEFINE(this, GrowOverTime)
        COMPONENT_FACTORY_DEFINE(this, Velocity)
        COMPONENT_FACTORY_DEFINE(this, Acceleration)

        COMPONENT_FACTORY_DEFINE(this, ParticleEmitter)
        COMPONENT_FACTORY_DEFINE(this, Material_Definition)
    }

    void
    EntityFactory::registerComponentFactory(const ComponentName &name,
                                            ComponentFactory &&cb) {
        assert(mComponentFactory.find(name.hash) == mComponentFactory.end());
        mComponentFactory.emplace(name.hash, std::move(cb));
    }

    void
    EntityFactory::registerEntityType_Canon(const EntityName &name) {
        // TODO: Redundent work, I should just pass the path to canon entity json files and let the class
        //  index the content.
        auto path = EntityDefDirPath{};
        path.path.append(mEntityResourcePath.path);
        path.path.append("canon/");
        path.path.append(name.str);
        path.path.append(".json");
        auto result = mEntityPathMap_Canon.emplace(name, std::move(path));
        assert(result.second);
    }

    void
    EntityFactory::registerEntityType_Extra(const EntityName &name) {
        auto path = EntityDefDirPath{};
        path.path.append(mEntityResourcePath.path);
        path.path.append("extra/");
        path.path.append(name.str);
        path.path.append(".json");
        auto result = mEntityPathMap_Extra.emplace(name, std::move(path));
        assert(result.second);
    }

    void
    EntityFactory::_postProcess(EntityManager &em,
                                EntityID id) {}

    const EntityDefDirPath &
    EntityFactory::_getEntityPath_Canon(const EntityName &name) {
        auto path = mEntityPathMap_Canon.find(name);
        if (path != mEntityPathMap_Canon.end()) {
            return path->second;
        }
        static auto empty = EntityDefDirPath{};
        return empty;
    }

    const EntityDefDirPath &
    EntityFactory::_getEntityPath_Extra(const EntityName &name) {
        auto path = mEntityPathMap_Extra.find(name);
        if (path != mEntityPathMap_Extra.end()) {
            return path->second;
        }
        assert(false);
        static auto empty = EntityDefDirPath{};
        return empty;
    }

    EntityID
    oni::EntityFactory::createEntity_Canon(EntityManager &manager,
                                           const EntityName &name) {
        auto fp = _getEntityPath_Canon(name);
        if (fp.path.empty()) {
            assert(false);
            return EntityManager::nullEntity();
        }

        auto maybeDoc = _readJson(fp);
        if (!maybeDoc.has_value()) {
            assert(false);
            return EntityManager::nullEntity();
        }
        auto document = std::move(maybeDoc.value());

        // TODO: Check for valid entityType?
        // TODO: After having registry take care of entity name string to entity type lookup I can
        // just add an overload to createEntity that receives EntityType_Name and returns and ID or nullEntity
        // if mapping is missing! Solves so many issues.
        // HMMM WHAT IS A VALID ENTITY NAME? If there is a file matching the requested name, then it must be
        // valid! Not sure if I need to care more than that :/
        auto id = manager.createEntity(name);

        auto components = document.FindMember("components");
        if (components != document.MemberEnd()) {
            for (auto component = components->value.MemberBegin();
                 component != components->value.MemberEnd(); ++component) {
                if (component->value.IsObject()) {
                    if (_createComponent(mComponentFactory, manager, id, document, component)) {
                        _postProcess(manager, id);
                    } else {
                        assert(false);
                    }
                } else {
                    assert(false);
                }
            }
        } else {
            assert(false);
        }
        return id;
    }

    void
    EntityFactory::createEntityExtras(EntityManager &mainEm,
                                      EntityManager &supportEm,
                                      EntityID id,
                                      const EntityName &name) {
        auto fp = _getEntityPath_Extra(name);
        if (fp.path.empty()) {
            return;
        }

        auto maybeDoc = _readJson(fp);
        if (!maybeDoc.has_value()) {
            assert(false);
            return;
        }
        auto document = std::move(maybeDoc.value());

        auto components = document.FindMember("components");
        if (components != document.MemberEnd()) {
            for (auto component = components->value.MemberBegin();
                 component != components->value.MemberEnd(); ++component) {
                if (component->value.IsObject()) {
                    if (_createComponent(mComponentFactory, supportEm, id, document, component)) {
                        _postProcess(supportEm, id);
                    } else {
                        assert(false);
                    }
                } else {
                    assert(false);
                }
            }
        } else {
            assert(false);
        }

        // TODO: Crazy idea, I could have a simple schema for this shit. Something like item path that looks like
        //  entities.entity.name: string. And pass that to a reader that returns the right item :h
        // TODO: This shit needs testing!
        auto entities = document.FindMember("entities");
        if (entities != document.MemberEnd()) {
            for (auto entity = entities->value.MemberBegin();
                 entity != entities->value.MemberEnd(); ++entity) {
                if (entity->value.IsObject()) {
                    auto entityNameObj = entity->value.FindMember("name");
                    if (entityNameObj->value.IsString()) {
                        auto entityName = entityNameObj->name.GetString();
                        auto _id = createEntity_Canon(supportEm, {EntityName::makeFromCStr(entityName)});
                        if (_id == EntityManager::nullEntity()) {
                            assert(false);
                            return;
                        }

                        auto bindLifetimeObj = entity->value.FindMember("bind-lifetime");
                        if (bindLifetimeObj->value.IsBool()) {
                            auto bindLifetime = bindLifetimeObj->value.GetBool();
                            if (bindLifetime) {
                                oni::EntityManager::bindLifetime({&mainEm, id}, {&supportEm, _id});
                            }
                        }
                    } else {
                        assert(false);
                    }
                } else {
                    assert(false);
                }
            }
        }
    }
}