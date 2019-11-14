#include <oni-core/entities/oni-entities-factory.h>

#include <cassert>

#include <oni-core/component/oni-component-visual.h>
#include <oni-core/entities/oni-entities-manager.h>
#include <oni-core/entities/oni-entities-serialization-hashed-string.h>
#include <oni-core/util/oni-util-enum.h>
#include <oni-core/json/oni-json.h>
#include <oni-core/io/oni-io-input-structure.h>
#include <oni-core/entities/oni-entities-serialization-json.h>


namespace {
    template<class C>
    C
    _readComponent(rapidjson::Value &data) {
        rapidjson::StringBuffer compStringBuff;
        rapidjson::Writer writer(compStringBuff);

        data.Accept(writer);

        std::istringstream ss(compStringBuff.GetString());
        cereal::JSONInputArchive reader(ss);
        auto c = C{};
        reader(c);
        return c;
    }

    void
    _createComponent(std::unordered_map<oni::Hash, oni::ComponentFactory> &factoryMap,
                     oni::EntityManager &manager,
                     oni::EntityID id,
                     rapidjson::Document &document,
                     const rapidjson::Document::MemberIterator &component) {
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
            std::istringstream ss(compStringBuff.GetString());
            cereal::JSONInputArchive reader(ss);
            // TODO: The issue with this right now is that if something is wrong in json I just get
            // an assert! What would happen in publish?
            factory->second(manager, id, reader);
        } else {
            assert(false);
        }
    }

    void
    _tryAttach(oni::EntityManager &parentEm,
               oni::EntityManager &childEm,
               oni::EntityID parentID,
               oni::EntityID childID,
               const rapidjson::Document::MemberIterator &data) {
        auto attached = data->value.FindMember("attached");
        if (attached != data->value.MemberEnd()) {
            if (attached->value.IsObject()) {
                auto attachedObj = attached->value.GetObject();
                oni::EntityManager::attach({&parentEm, parentID}, {&childEm, childID});

                auto pos = _readComponent<oni::WorldP3D>(attached->value);
                parentEm.setWorldP3D(childID, pos.x, pos.y, pos.z);
            } else {
                assert(false);
            }
        }
    }

    void
    _tryBindLifeTime(
            oni::EntityManager &parentEm,
            oni::EntityManager &childEm,
            oni::EntityID parentID,
            oni::EntityID childID,
            const rapidjson::Document::MemberIterator &data) {
        auto bindLifetimeObj = data->value.FindMember("bind-lifetime");
        if (bindLifetimeObj->value.IsBool()) {
            auto bindLifetime = bindLifetimeObj->value.GetBool();
            if (bindLifetime) {
                oni::EntityManager::bindLifetime({&parentEm, parentID}, {&childEm, childID});
            }
        } else {
            assert(false);
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
        COMPONENT_FACTORY_DEFINE(this, SoundPitch)

        COMPONENT_FACTORY_DEFINE(this, PhysicalProperties)
        COMPONENT_FACTORY_DEFINE(this, CarInput)
        COMPONENT_FACTORY_DEFINE(this, Car)
        COMPONENT_FACTORY_DEFINE(this, CarConfig)

        COMPONENT_FACTORY_DEFINE(this, ParticleEmitter)
        COMPONENT_FACTORY_DEFINE(this, Material_Definition)
        COMPONENT_FACTORY_DEFINE(this, Material_Text)
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
        assert(false);
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
        auto maybeDoc = readJson(fp);
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
        auto parentID = manager.createEntity(name);

        auto components = document.FindMember("components");
        if (components != document.MemberEnd()) {
            for (auto component = components->value.MemberBegin();
                 component != components->value.MemberEnd(); ++component) {
                if (component->value.IsObject()) {
                    _createComponent(mComponentFactory, manager, parentID, document, component);
                } else {
                    assert(false);
                }
            }
        } else {
            assert(false);
        }
        _postProcess(manager, parentID);

        auto entities = document.FindMember("entities");
        if (entities != document.MemberEnd()) {
            for (auto entity = entities->value.MemberBegin();
                 entity != entities->value.MemberEnd(); ++entity) {
                if (entity->value.IsObject()) {
                    auto entityNameObj = entity->value.FindMember("name");
                    if (entityNameObj->value.IsString()) {
                        auto entityName = entityNameObj->value.GetString();
                        auto childID = createEntity_Canon(manager, {EntityName::makeFromCStr(entityName)});
                        if (childID == EntityManager::nullEntity()) {
                            assert(false);
                        }

                        _tryAttach(manager, manager, parentID, childID, entity);
                    } else {
                        assert(false);
                    }
                } else {
                    assert(false);
                }
            }
        }

        return parentID;
    }

    void
    EntityFactory::createEntity_Extras(EntityManager &parentEm,
                                       EntityManager &childEm,
                                       EntityID parentID,
                                       const EntityName &name) {
        auto fp = _getEntityPath_Extra(name);
        if (fp.path.empty()) {
            return;
        }

        auto maybeDoc = readJson(fp);
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
                    _createComponent(mComponentFactory, parentEm, parentID, document, component);
                } else {
                    assert(false);
                }
            }
        } else {
            assert(false);
        }

        _postProcess(parentEm, parentID);

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
                        auto entityName = entityNameObj->value.GetString();
                        auto childID = createEntity_Canon(childEm, {EntityName::makeFromCStr(entityName)});
                        if (childID == EntityManager::nullEntity()) {
                            assert(false);
                            return;
                        }
                        _tryBindLifeTime(parentEm, childEm, parentID, childID, entity);
                        _tryAttach(parentEm, childEm, parentID, childID, entity);

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