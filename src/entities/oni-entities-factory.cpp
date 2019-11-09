#include <oni-core/entities/oni-entities-factory.h>

#include <cassert>

#include <oni-core/component/oni-component-visual.h>
#include <oni-core/entities/oni-entities-manager.h>
#include <oni-core/entities/oni-entities-serialization-hashed-string.h>
#include <oni-core/util/oni-util-enum.h>
#include <oni-core/json/oni-json.h>
#include <oni-core/io/oni-io-input-structure.h>


namespace {
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
            std::istringstream ss;
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

namespace oni {
    template<class Archive>
    void
    serialize(Archive &archive,
              CarConfig &data) {
        archive("gravity", data.gravity);
        archive("mass", data.mass);
        archive("inertialScale", data.inertialScale);
        archive("halfWidth", data.halfWidth);
        archive("cgToFront", data.cgToFront);
        archive("cgToRear", data.cgToRear);
        archive("cgToFrontAxle", data.cgToFrontAxle);
        archive("cgToRearAxle", data.cgToRearAxle);
        archive("cgHeight", data.cgHeight);
        archive("wheelRadius", data.wheelRadius);
        archive("wheelWidth", data.wheelWidth);
        archive("tireGrip", data.tireGrip);
        archive("lockGrip", data.lockGrip);
        archive("engineForce", data.engineForce);
        archive("brakeForce", data.brakeForce);
        archive("eBrakeForce", data.eBrakeForce);
        archive("weightTransfer", data.weightTransfer);
        archive("maxSteer", data.maxSteer);
        archive("cornerStiffnessFront", data.cornerStiffnessFront);
        archive("cornerStiffnessRear", data.cornerStiffnessRear);
        archive("airResist", data.airResist);
        archive("rollResist", data.rollResist);
        archive("gearRatio", data.gearRatio);
        archive("differentialRatio", data.differentialRatio);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              Car &data) {
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              WorldP3D &data) {
        archive("y", data.x);
        archive("y", data.y);
        archive("z", data.z);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              WorldP2D &data) {
        archive("x", data.x);
        archive("y", data.y);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              Orientation &data) {
        archive("value", data.value);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              Scale &data) {
        archive("x", data.x);
        archive("y", data.y);
        archive("z", data.z);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              TimeToLive &data) {
        archive("currentAge", data.currentAge);
        archive("maxAge", data.maxAge);
    }

    template<class Archive>
    void
    save(Archive &archive,
         const Color &data) {
        auto r = data.r_r32();
        auto g = data.r_r32();
        auto b = data.r_r32();
        auto a = data.r_r32();
        archive("r", r);
        archive("g", g);
        archive("b", b);
        archive("a", a);
    }

    template<class Archive>
    void
    load(Archive &archive,
         Color &data) {
        auto color = vec4();
        archive("r", color.x);
        archive("g", color.y);
        archive("b", color.z);
        archive("a", color.w);
        data.set_rgba(color.x, color.y, color.z, color.w);
    }

    template<class Archive>
    void
    save(Archive &archive,
         const Image &data) {
        saveHashedString(archive, "name", data.name);
    }

    template<class Archive>
    void
    load(Archive &archive,
         Image &data) {
        loadHashedString(archive, "name", data.name);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              Texture &data) {
        archive("image", data.image);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              Material_Skin &data) {
        archive("color", data.color);
        archive("texture", data.texture);
    }

    template<class Archive>
    void
    save(Archive &archive,
         const Material_Finish &data) {
        saveEnum(archive, "name", data);
    }

    template<class Archive>
    void
    load(Archive &archive,
         Material_Finish &data) {
        loadEnum(archive, "name", data);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              Material_Definition &data) {
        archive("finish", data.finish);
        archive("skin", data.skin);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              GrowOverTime &data) {
        archive("period", data.period);
        archive("elapsed", data.elapsed);
        archive("factor", data.factor);
        archive("maxSize", data.maxSize);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              ParticleEmitter &data) {
        archive("material", data.material);
        archive("size", data.size);
        archive("initialVMin", data.initialVMin);
        archive("initialVMax", data.initialVMax);
        archive("orientMin", data.orientMin);
        archive("orientMax", data.orientMax);
        archive("acc", data.acc);
        archive("count", data.count);
        archive("growth", data.growth);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              AfterMark &data) {
        archive("material", data.material);
        archive("type", data.type);
        archive("size", data.scale);
    }

    template<class Archive>
    void
    save(Archive &archive,
         const Sound &data) {
        saveHashedString(archive, "name", data.name);
        archive("group", data.group);
    }

    template<class Archive>
    void
    load(Archive &archive,
         Sound &data) {
        loadHashedString(archive, "name", data.name);
        archive("group", data.group);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              SoundPitch &data) {
        archive("value", data.value);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              Direction &data) {
        archive("x", data.x);
        archive("y", data.y);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              Velocity &data) {
        archive("current", data.current);
        archive("max", data.max);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              Acceleration &data) {
        archive("current", data.current);
        archive("max", data.max);
    }

    template<class Archive>
    void
    save(Archive &archive,
         const BodyType &data) {
        saveEnum(archive, "name", data);
    }

    template<class Archive>
    void
    load(Archive &archive,
         BodyType &data) {
        loadEnum(archive, "name", data);
    }

    template<class Archive>
    void
    save(Archive &archive,
         const PhysicalCategory &data) {
        saveEnum(archive, "name", data);
    }

    template<class Archive>
    void
    load(Archive &archive,
         PhysicalCategory &data) {
        loadEnum(archive, "name", data);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              PhysicalProperties &data) {
        archive("linearDamping", data.linearDamping);
        archive("angularDamping", data.angularDamping);
        archive("density", data.density);
        archive("friction", data.friction);
        archive("gravityScale", data.gravityScale);
        archive("highPrecision", data.highPrecision);
        archive("collisionWithinCat", data.collisionWithinCat);
        archive("isSensor", data.isSensor);
        archive("bodyType", data.bodyType);
        archive("physicalCat", data.physicalCat);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              CarInput &data) {
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
        auto id = manager.createEntity(name);

        auto components = document.FindMember("components");
        if (components != document.MemberEnd()) {
            for (auto component = components->value.MemberBegin();
                 component != components->value.MemberEnd(); ++component) {
                if (component->value.IsObject()) {
                    _createComponent(mComponentFactory, manager, id, document, component);
                } else {
                    assert(false);
                }
            }
        } else {
            assert(false);
        }
        _postProcess(manager, id);
        return id;
    }

    void
    EntityFactory::createEntity_Extras(EntityManager &mainEm,
                                       EntityManager &supportEm,
                                       EntityID id,
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
                    _createComponent(mComponentFactory, mainEm, id, document, component);
                } else {
                    assert(false);
                }
            }
        } else {
            assert(false);
        }

        _postProcess(mainEm, id);

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