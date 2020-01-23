#pragma once

#include <oni-core/component/oni-component-visual.h>
#include <oni-core/component/oni-component-physics.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/graphic/oni-graphic-camera.h>
#include <oni-core/component/oni-component-audio.h>
#include <oni-core/entities/oni-entities-serialization-hashed-string.h>
#include <oni-core/io/oni-io-input-structure.h>

namespace oni {
    template<class Archive, class ENUM>
    void
    loadEnum(Archive &archive,
             std::string_view name,
             ENUM &data) {
        if (name.empty()) {
            assert(false);
            return;
        }

        std::string buffer{};
        archive(name.data(), buffer);
        auto hash = oni::HashedString::makeHashFromCString(buffer.data());
        // NOTE: After initialization hash will point to static storage so buffer is safe to go out of scope
        data.runtimeInit(hash);
    }

    // NOTE: This version is for json
    template<class Archive, class ENUM>
    void
    saveEnum(Archive &archive,
             std::string_view name,
             ENUM &data) {
        if (name.empty()) {
            assert(false);
            return;
        }
        auto buffer = std::string(data.name.str);
        archive(name.data(), buffer);
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
        archive("x", data.x);
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
    serialize(Archive &archive,
              Material_Text &data) {
        // TODO:
        // assert(false);
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
        archive("particle", data.particle);
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
    serialize(Archive &archive,
              JetForce &data) {
        archive("fuze", data.fuze);
        archive("force", data.force);
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

    template<class Archive>
    void
    save(Archive &archive,
         const ZLayer &data) {
        saveHashedString(archive, "value", data.name);
    }

    template<class Archive>
    void
    load(Archive &archive,
         ZLayer &data) {
        loadEnum(archive, "value", data);
        auto buffer = ZLayer{};
        oni::loadEnum(archive, "value", buffer);
        data = buffer;
    }
}