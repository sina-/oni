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
#include <cereal/external/rapidjson/error/en.h>


namespace {
    oni::EntityManager *
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
    _readComponent(const oni::ComponentReaderMap &readerMap,
                   oni::EntityManager &manager,
                   oni::EntityID id,
                   rapidjson::Document &document,
                   const rapidjson::Document::MemberIterator &component) {
        auto compoName = component->name.GetString();
        auto hash = oni::HashedString::makeHashFromCString(compoName);

        rapidjson::StringBuffer compStringBuff;
        rapidjson::Writer writer(compStringBuff);

        // NOTE: I need to recreate the object because rapidjson component splits up component name
        // and data but cereal expects a json object where the component name is the key for
        // the object in the form: {"Velocity": {"x": 1, "y": 1, "z": 1}}
        auto data = rapidjson::Value(rapidjson::kObjectType);
        data.AddMember(component->name.Move(), component->value.Move(), document.GetAllocator());
        data.Accept(writer);

        auto factory = readerMap.find(hash);
        if (factory != readerMap.end()) {
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
    _writeComponent(const oni::ComponentWriterMap &writerMap,
                    const oni::EntityManager &manager,
                    oni::EntityID id,
                    std::string_view compoName,
                    std::stringstream &ss) {
        auto hash = oni::HashedString::makeHashFromCString(compoName.data());
        auto factory = writerMap.find(hash);

        static auto options = cereal::JSONOutputArchive::Options(4,
                                                                 cereal::JSONOutputArchive::Options::IndentChar::space,
                                                                 2);
        if (factory != writerMap.end()) {
            cereal::JSONOutputArchive writer(ss, options);
            factory->second(manager, id, writer);
        } else {
            assert(false);
        }
    }

    template<class C>
    C
    _readComponent(const oni::ComponentReaderMap &readerMap,
                   rapidjson::Document &doc,
                   const rapidjson::Document::MemberIterator &component) {
        auto *em = _getTempEntityManager();
        auto id = _getTempEntityID(em);
        _readComponent(readerMap, *em, id, doc, component);
        auto result = em->get<C>(id);
        em->removeComponent<C>(id);
        return result;
    }

    oni::EntityName
    _readEntityName(const oni::c8 *strValue,
                    oni::EntityFactory &factory,
                    rapidjson::Document &doc) {
        // TODO: LOL so much bullshit to create the json structs wtf.
        auto value = rapidjson::Value(rapidjson::kObjectType);
        value.AddMember("value", rapidjson::Value(strValue, doc.GetAllocator()).Move(),
                        doc.GetAllocator());
        auto entityNameJson = rapidjson::Value(rapidjson::kObjectType);
        entityNameJson.AddMember("EntityName",
                                 value.Move(),
                                 doc.GetAllocator());
        auto entityName = _readComponent<oni::EntityName>(factory.getFactoryMap(), doc,
                                                          entityNameJson.MemberBegin());
        return entityName;
    }

    void
    _readComponents(const oni::ComponentReaderMap &readerMap,
                    std::string_view key,
                    oni::EntityManager &em,
                    oni::EntityID id,
                    rapidjson::Document &doc) {
        auto components = doc.FindMember(key.data());
        if (components != doc.MemberEnd()) {
            for (auto component = components->value.MemberBegin();
                 component != components->value.MemberEnd(); ++component) {
                if (component->value.IsObject()) {
                    _readComponent(readerMap, em, id, doc, component);
                } else {
                    assert(false);
                }
            }
        } else {
            assert(false);
        }
    }

    void
    _writeComponents(const oni::ComponentWriterMap &writerMap,
                     std::string_view key,
                     const oni::EntityManager &em,
                     oni::EntityID id,
                     const rapidjson::Document &doc,
                     rapidjson::Document &result) {
        auto components = doc.FindMember(key.data());
        if (components != doc.MemberEnd()) {
            for (auto component = components->value.MemberBegin();
                 component != components->value.MemberEnd(); ++component) {
                if (component->value.IsObject()) {
                    auto ss = std::stringstream();
                    _writeComponent(writerMap, em, id, component->name.GetString(), ss);

                    auto componentDoc = rapidjson::Document(&result.GetAllocator());
                    auto componentString = std::string(ss.str());
                    if (componentDoc.Parse(componentString.data()).HasParseError()) {
                        printf("JSON parse error: %s file: position: %zu \n",
                               rapidjson::GetParseError_En(componentDoc.GetParseError()),
                               componentDoc.GetErrorOffset());
                        assert(false);
                        return;
                    }
                    result["components-local"].PushBack(componentDoc, result.GetAllocator());

                } else {
                    assert(false);
                }
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
    _createEntity_Attachments(oni::EntityFactory &factory,
                              std::string_view key,
                              oni::EntityManager &primaryEm,
                              oni::EntityManager &secondaryEm,
                              oni::EntityID parentID,
                              rapidjson::Document &doc) {
        // TODO: Crazy idea, I could have a simple schema for this shit. Something like item path that looks like
        //  entities.entity.name: string. And pass that to a reader that returns the right item :h
        auto entities = doc.FindMember(key.data());
        if (entities != doc.MemberEnd()) {
            for (auto entity = entities->value.MemberBegin();
                 entity != entities->value.MemberEnd(); ++entity) {
                if (entity->value.IsObject()) {
                    auto entityNameObj = entity->value.FindMember("name");
                    if (entityNameObj->value.IsString()) {
                        auto entityNameStr = entityNameObj->value.GetString();
                        auto entityName = _readEntityName(entityNameStr, factory, doc);
                        // NOTE: If secondary entity requires entities as well, those will always will be
                        // created in the secondary entity registry.
                        auto childID = factory.readEntity_Local(secondaryEm, secondaryEm, entityName);

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
    oni::EntityFactory::EntityFactory(ZLayerManager &zLayer) : mZLayerManager(zLayer) {
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
    EntityFactory::indexEntities(EntityDefDirPath &&fp) {
        auto entities = parseDirectoryTree(fp);

        for (auto &&file: entities) {
            auto doc = rapidjson::Document();
            auto entityName = _readEntityName(file.name.data(), *this, doc);
            auto result = mEntityPathMap.emplace(entityName, EntityDefDirPath{file});
            assert(result.second);
        }
    }

    void
    EntityFactory::registerComponentReader(const ComponentName &name,
                                           ComponentReader &&cr) {
        auto result = mComponentReader.emplace(name.hash, std::move(cr));
        assert(result.second);
    }

    void
    EntityFactory::registerComponentWriter(const ComponentName &name,
                                           ComponentWriter &&cw) {
        auto result = mComponentWriter.emplace(name.hash, std::move(cw));
        assert(result.second);
    }

    const ComponentReaderMap &
    EntityFactory::getFactoryMap() const {
        return mComponentReader;
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
    EntityFactory::_getEntityPath(const EntityName &name) {
        auto path = mEntityPathMap.find(name);
        if (path != mEntityPathMap.end()) {
            return path->second;
        }
        assert(false);
        static auto empty = EntityDefDirPath{};
        return empty;
    }

    EntityID
    EntityFactory::readEntity_Local(EntityManager &primaryEm,
                                    EntityManager &secondaryEm,
                                    const EntityName &name) {
        auto fp = _getEntityPath(name);
        auto parentID = primaryEm.createEntity(name);
        auto maybeDoc = readJson(fp);
        if (!maybeDoc.has_value()) {
            assert(false);
            return EntityManager::nullEntity();
        }
        auto doc = std::move(maybeDoc.value());

        _readComponents(mComponentReader, "components-local", primaryEm, parentID, doc);
        _postProcess(primaryEm, parentID);
        _createEntity_Attachments(*this, "attachments-local", primaryEm, secondaryEm, parentID, doc);

        return parentID;
    }

    void
    EntityFactory::readEntity_Remote(EntityManager &primaryEm,
                                     EntityManager &secondaryEm,
                                     EntityID parentID,
                                     const EntityName &name) {
        auto fp = _getEntityPath(name);
        if (fp.path.empty()) {
            return;
        }

        auto maybeDoc = readJson(fp);
        if (!maybeDoc.has_value()) {
            assert(false);
            return;
        }
        auto doc = std::move(maybeDoc.value());

        _readComponents(mComponentReader, "components-remote", primaryEm, parentID, doc);
        _postProcess(primaryEm, parentID);
        _createEntity_Attachments(*this, "attachments-remote", primaryEm, secondaryEm, parentID, doc);
    }

    void
    EntityFactory::writeEntity_Local(const EntityManager &primaryEm,
                                     EntityID id,
                                     const EntityName &name) {
        auto fp = _getEntityPath(name);
        auto maybeDoc = readJson(fp);
        if (!maybeDoc.has_value()) {
            assert(false);
            return;
        }
        auto doc = std::move(maybeDoc.value());

        auto result = rapidjson::Document();
        result.SetObject();

        result.AddMember("name", rapidjson::Value(rapidjson::kStringType).Move(), result.GetAllocator());
        result["name"] = rapidjson::StringRef(name.name.str.data());

        result.AddMember("components-local", rapidjson::Value(rapidjson::kArrayType).Move(), result.GetAllocator());
        _writeComponents(mComponentWriter, "components-local", primaryEm, id, doc, result);

        auto outputFP = FilePath("", "test", "json");
        writeJson(outputFP, result);
    }
}