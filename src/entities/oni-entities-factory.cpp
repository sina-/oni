#include <oni-core/entities/oni-entities-factory.h>

#include <cassert>

#include <oni-core/component/oni-component-visual.h>
#include <oni-core/entities/oni-entities-manager.h>
#include <oni-core/entities/oni-entities-serialization-hashed-string.h>
#include <oni-core/util/oni-util-enum.h>
#include <oni-core/json/oni-json.h>
#include <oni-core/io/oni-io-input-structure.h>
#include <oni-core/entities/oni-entities-serialization-json.h>
#include <oni-core/graphic/oni-graphic-camera.h>
#include <oni-core/math/oni-math-z-layer-manager.h>


namespace {
    static oni::EntityManager *
    _getTempEntityManager() {
        static auto em = new oni::EntityManager(oni::SimMode::SERVER, nullptr);
        return em;
    }

    oni::EntityID
    _getTempEntityID(oni::EntityManager *em) {
        static auto id = em->createEntity();
        return id;
    }

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
    _createComponent(const oni::ComponentFactoryMap &factoryMap,
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

    template<class C>
    C
    _readComponent(const oni::ComponentFactoryMap &factoryMap,
                   rapidjson::Document &doc,
                   const rapidjson::Document::MemberIterator &component) {
        auto *em = _getTempEntityManager();
        auto id = _getTempEntityID(em);
        _createComponent(factoryMap, *em, id, doc, component);
        auto result = em->get<C>(id);
        em->removeComponent<C>(id);
        return result;
    }

    void
    _createComponents(const oni::ComponentFactoryMap &factoryMap,
                      oni::EntityManager &em,
                      oni::EntityID id,
                      rapidjson::Document &doc,
                      const rapidjson::Document::MemberIterator &components) {
        for (auto component = components->value.MemberBegin();
             component != components->value.MemberEnd(); ++component) {
            if (component->value.IsObject()) {
                _createComponent(factoryMap, em, id, doc, component);
            } else {
                assert(false);
            }
        }
    }

    void
    _createComponents_Primary(const oni::ComponentFactoryMap &factoryMap,
                              oni::EntityManager &em,
                              oni::EntityID id,
                              rapidjson::Document &doc) {
        auto primary = doc.FindMember("primary");
        if (primary != doc.MemberEnd()) {
            auto components = primary->value.FindMember("components");
            if (components != primary->value.MemberEnd()) {
                _createComponents(factoryMap, em, id, doc, components);
            } else {
                assert(false);
            }
        } else {
            assert(false);
        }
    }


    void
    _tryAttach(oni::EntityManager &parentEm,
               oni::EntityManager &childEm,
               oni::EntityID parentID,
               oni::EntityID childID,
               const rapidjson::Document::MemberIterator &iter) {
        auto attached = iter->value.FindMember("attached");
        if (attached != iter->value.MemberEnd()) {
            if (attached->value.IsObject()) {
                auto attachedObj = attached->value.GetObject();
                oni::EntityManager::attach({&parentEm, parentID}, {&childEm, childID});

                auto pos = _readComponent<oni::WorldP3D>(attached->value);
                childEm.setWorldP3D(childID, pos.x, pos.y, pos.z);
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
            const rapidjson::Document::MemberIterator &iter) {
        auto bindLifetimeObj = iter->value.FindMember("bind-lifetime");
        if (bindLifetimeObj != iter->value.MemberEnd()) {
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

    void
    _createEntity_Secondary(oni::EntityFactory &factory,
                            oni::EntityManager &primaryEm,
                            oni::EntityManager &secondaryEm,
                            oni::EntityID parentID,
                            rapidjson::Document &doc) {
        // TODO: Crazy idea, I could have a simple schema for this shit. Something like item path that looks like
        //  entities.entity.name: string. And pass that to a reader that returns the right item :h
        auto entities = doc.FindMember("secondary");
        if (entities != doc.MemberEnd()) {
            for (auto entity = entities->value.MemberBegin();
                 entity != entities->value.MemberEnd(); ++entity) {
                if (entity->value.IsObject()) {
                    auto entityNameObj = entity->value.FindMember("name");
                    if (entityNameObj->value.IsString()) {
                        // TODO: LOL so much bullshit to create the json structs wtf.
                        auto entityNameStr = entityNameObj->value.GetString();
                        auto value = rapidjson::Value(rapidjson::kObjectType);
                        value.AddMember("value", rapidjson::Value(entityNameStr, doc.GetAllocator()).Move(),
                                        doc.GetAllocator());
                        auto entityNameJson = rapidjson::Value(rapidjson::kObjectType);
                        entityNameJson.AddMember("EntityName",
                                                 value.Move(),
                                                 doc.GetAllocator());

                        auto entityName = _readComponent<oni::EntityName>(factory.getFactoryMap(), doc,
                                                                          entityNameJson.MemberBegin());
                        // NOTE: If secondary entity requires entities as well, those will always will be
                        // created in the secondary entity registry.
                        auto childID = factory.createEntity_Primary(secondaryEm, secondaryEm, entityName);

                        if (childID == oni::EntityManager::nullEntity()) {
                            assert(false);
                            continue;
                        }
                        _tryBindLifeTime(primaryEm, secondaryEm, parentID, childID, entity);
                        _tryAttach(primaryEm, secondaryEm, parentID, childID, entity);
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

namespace oni {
    oni::EntityFactory::EntityFactory(EntityDefDirPath &&fp,
                                      ZLayerManager &zLayer) : mEntityResourcePath(std::move(fp)),
                                                               mZLayerManager(zLayer) {
        // TODO: This is really dependent on the user, remove it.
        COMPONENT_FACTORY_DEFINE(this, oni, WorldP3D)
        COMPONENT_FACTORY_DEFINE(this, oni, WorldP2D)
        COMPONENT_FACTORY_DEFINE(this, oni, ZLayer)
        COMPONENT_FACTORY_DEFINE(this, oni, Direction)
        COMPONENT_FACTORY_DEFINE(this, oni, Orientation)
        COMPONENT_FACTORY_DEFINE(this, oni, Scale)
        COMPONENT_FACTORY_DEFINE(this, oni, GrowOverTime)
        COMPONENT_FACTORY_DEFINE(this, oni, Velocity)
        COMPONENT_FACTORY_DEFINE(this, oni, Acceleration)

        COMPONENT_FACTORY_DEFINE(this, oni, SoundPitch)

        COMPONENT_FACTORY_DEFINE(this, oni, CarInput)
        COMPONENT_FACTORY_DEFINE(this, oni, Car)
        COMPONENT_FACTORY_DEFINE(this, oni, CarConfig)
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
        path.path.append("primary/");
        path.path.append(name.name.str);
        path.path.append(".json");
        auto result = mEntityPathMap_Canon.emplace(name, std::move(path));
        assert(result.second);
    }

    void
    EntityFactory::registerEntityType_Extra(const EntityName &name) {
        auto path = EntityDefDirPath{};
        path.path.append(mEntityResourcePath.path);
        path.path.append("secondary/");
        path.path.append(name.name.str);
        path.path.append(".json");
        auto result = mEntityPathMap_Extra.emplace(name, std::move(path));
        assert(result.second);
    }

    const ComponentFactoryMap &
    EntityFactory::getFactoryMap() const {
        return mComponentFactory;
    }

    void
    EntityFactory::_postProcess(EntityManager &em,
                                EntityID id) {
        if (em.has<ZLayer>(id)) {
            auto &zLayer = em.get<ZLayer>(id);
            auto &pos = em.get<WorldP3D>(id);
            pos.z = mZLayerManager.getZAt(zLayer);
        }
    }

    const EntityDefDirPath &
    EntityFactory::_getEntityPath_Primary(const EntityName &name) {
        auto path = mEntityPathMap_Canon.find(name);
        if (path != mEntityPathMap_Canon.end()) {
            return path->second;
        }
        assert(false);
        static auto empty = EntityDefDirPath{};
        return empty;
    }

    const EntityDefDirPath &
    EntityFactory::_getEntityPath_Secondary(const EntityName &name) {
        auto path = mEntityPathMap_Extra.find(name);
        if (path != mEntityPathMap_Extra.end()) {
            return path->second;
        }
        assert(false);
        static auto empty = EntityDefDirPath{};
        return empty;
    }

    EntityID
    EntityFactory::createEntity_Primary(EntityManager &primaryEm,
                                        EntityManager &secondaryEm,
                                        const EntityName &name) {
        auto fp = _getEntityPath_Primary(name);
        auto parentID = primaryEm.createEntity(name);
        auto maybeDoc = readJson(fp);
        if (!maybeDoc.has_value()) {
            assert(false);
            return EntityManager::nullEntity();
        }
        auto doc = std::move(maybeDoc.value());

        _createComponents_Primary(mComponentFactory, primaryEm, parentID, doc);
        _postProcess(primaryEm, parentID);
        _createEntity_Secondary(*this, primaryEm, secondaryEm, parentID, doc);

        return parentID;
    }

    void
    EntityFactory::createEntity_Secondary(EntityManager &primaryEm,
                                          EntityManager &secondaryEm,
                                          EntityID parentID,
                                          const EntityName &name) {
        auto fp = _getEntityPath_Secondary(name);
        if (fp.path.empty()) {
            return;
        }

        auto maybeDoc = readJson(fp);
        if (!maybeDoc.has_value()) {
            assert(false);
            return;
        }
        auto doc = std::move(maybeDoc.value());

        _createComponents_Primary(mComponentFactory, primaryEm, parentID, doc);
        _postProcess(primaryEm, parentID);
        _createEntity_Secondary(*this, primaryEm, secondaryEm, parentID, doc);
    }
}