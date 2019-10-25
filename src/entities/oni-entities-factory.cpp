#include <oni-core/entities/oni-entities-factory.h>

// NOTE: For cereal to not throw exceptions and disable stupid noexcept constructor that throws in debug!
#define JSON_NOEXCEPTION

#include <fstream>
#include <istream>
#include <cereal/archives/json.hpp>

#include <oni-core/component/oni-component-visual.h>
#include <oni-core/entities/oni-entities-manager.h>
#include <oni-core/entities/oni-entities-serialization.h>
#include <oni-core/graphic/oni-graphic-texture-manager.h>


#define COMPONENT_FACTORY_DEFINE(factory, COMPONENT_NAME)                           \
{                                                                                   \
        ComponentFactory cf = [](EntityManager &em,                                 \
                                 EntityID id,                                       \
                                 cereal::JSONInputArchive &reader) {                \
            auto &component = em.createComponent<COMPONENT_NAME>(id);               \
            reader(component);                                                      \
        };                                                                          \
        auto componentName = Component_Name{HashedString(#COMPONENT_NAME)};         \
        factory->registerComponentFactory(componentName, std::move(cf));            \
}

namespace oni {
    oni::EntityFactory::EntityFactory(FilePath fp,
                                      TextureManager &tm) : mEntityResourcePath(std::move(fp)),
                                                            mTextureManager(tm) {
        // TODO: Same thing happens in AssetManager, can I move this to a single location?
        if (mEntityResourcePath.value.back() != '/') {
            mEntityResourcePath.value.append("/");
        }

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
    EntityFactory::registerComponentFactory(const Component_Name &name,
                                            ComponentFactory &&cb) {
        assert(mComponentFactory.find(name.value.hash) == mComponentFactory.end());
        mComponentFactory.emplace(name.value.hash, std::move(cb));
    }

    void
    EntityFactory::registerEntityType(const EntityType_Name &name) {
        auto path = FilePath{};
        path.value.append(mEntityResourcePath.value);
        path.value.append(name.value.str);
        path.value.append(".json");
        mEntityResourcePathLookup.emplace(name.value.hash, std::move(path));
    }

    const FilePath &
    EntityFactory::getEntityResourcePath(const EntityType_Name &name) {
        auto path = mEntityResourcePathLookup.find(name.value.hash);
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
        auto id = manager.createEntity(name);

        auto jsonString = std::string((std::istreambuf_iterator<char>(jsonStream)), std::istreambuf_iterator<char>());

        auto document = rapidjson::Document();
        if (document.Parse(jsonString.c_str()).HasParseError()) {
            assert(false);
            return EntityManager::nullEntity();
        }

        if (document.IsObject()) {
            auto components = document.FindMember("components");
            if (components != document.MemberEnd()) {
                for (auto component = components->value.MemberBegin();
                     component != components->value.MemberEnd(); ++component) {
                    if (component->value.IsObject()) {
                        auto compoName = component->name.GetString();

                        rapidjson::StringBuffer compStringBuff;
                        rapidjson::Writer writer(compStringBuff);

                        // NOTE: I need to recreate the object because rapidjson component splits up component name
                        // and data but cereal expects a json object where the component name is the key for
                        // the object in the form: {"Velocity": {"x": 1, "y": 1, "z": 1}}
                        auto data = rapidjson::Value(rapidjson::kObjectType);
                        data.AddMember(component->name.Move(), component->value.Move(), document.GetAllocator());
                        data.Accept(writer);

                        auto hashedString = HashedString(compoName);
                        auto factory = mComponentFactory.find(hashedString.hash);
                        if (factory != mComponentFactory.end()) {
                            // TODO: so many stream and conversions, can't I just use the stream I get from rapidjson?
                            std::istringstream ss;
                            ss.str(compStringBuff.GetString());
                            cereal::JSONInputArchive reader(ss);
                            // TODO: The issue with this right now is that if something is wrong in json I just get
                            // an assert! What would happen in publish?
                            factory->second(manager, id, reader);

                            postProcess(manager, id);
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
        } else {
            assert(false);
        }
        return id;
    }

    void
    EntityFactory::postProcess(EntityManager &em,
                               EntityID id) {
        if (em.has<Material_Definition>(id)) {
            auto &def = em.get<Material_Definition>(id);
            mTextureManager.initTexture(def.skin.texture);
        }
    }
}