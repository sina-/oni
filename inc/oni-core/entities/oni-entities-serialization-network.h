#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/component/oni-component-audio.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-visual.h>
#include <oni-core/entities/oni-entities-serialization-hashed-string.h>
#include <oni-core/entities/oni-entities-structure.h>
#include <oni-core/game/oni-game-event.h>
#include <oni-core/network/oni-network-packet.h>

namespace oni {
    // NOTE: This version is for json where a user defined ENUM of EnumBase sub-type is required
    // NOTE: This is for network serialization where we have a specific ENUM with correct id
    template<class Archive, class ENUM>
    void
    loadEnum(Archive &archive,
             ENUM &data) {
        archive(data.id);
        loadHashedString(archive, 0, data.name);
    }

    // NOTE: This is for network serialization where we have a specific ENUM with correct id
    template<class Archive, class ENUM>
    void
    saveEnum(Archive &archive,
             ENUM &data) {
        archive(data.id);
        saveHashedString(archive, 0, data.name);
    }
}

namespace oni {
    // NOTE: These functions can't be defined in serialization.cpp since that would make them invisible to users in
    // other compilation units such as server.cpp

    template<class Archive>
    void
    serialize(Archive &archive,
              mat4 &data) {
        archive(data.elements);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              vec2 &data) {
        archive(data.x, data.y);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              vec3 &data) {
        archive(data.x, data.y, data.z);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              vec4 &data) {
        archive(data.x, data.y, data.z, data.w);
    }

    template<class Archive>
    void
    save(Archive &archive,
         const EntityName &data) {
        saveEnum(archive, data);
    }

    template<class Archive>
    void
    load(Archive &archive,
         EntityName &data) {
        loadEnum(archive, data);
    }

    template<class Archive>
    void
    save(Archive &archive,
         const DeletedEntity &data) {
        archive(data.id);
        archive(data.name);
    }

    template<class Archive>
    void
    load(Archive &archive,
         DeletedEntity &data) {
        archive(data.id);
        archive(data.name);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              WorldP3D &data) {
        archive(data.x);
        archive(data.y);
        archive(data.z);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              Packet_Data &packet) {
        archive(packet.data);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              Packet_EntityID &packet) {
        archive(packet.entity);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              Impulse2D &data) {
        archive(data.value, data.normal);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              EntityPair &data) {
        archive(data.a, data.b);
    }

    template<class Archive>
    void
    save(Archive &archive,
         const PhysicalCategory &data) {
        saveEnum(archive, data);
    }

    template<class Archive>
    void
    load(Archive &archive,
         PhysicalCategory &data) {
        loadEnum(archive, data);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              PhysicalCatPair &data) {
        archive(data.a, data.b);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              Event_Collision &data) {
        // NOTE: EntityPair is not serialized as this is server specific, although I could send it to the client
        // and do a mapping prior to use if need be!
        archive(data.pos, data.impulse, data.pcPair);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              Event_RocketLaunch &data) {
        archive(data.pos);
    }

    template<class Archive>
    void
    save(Archive &archive,
         const Event_SoundPlay &data) {
        saveHashedString(archive, 0, data.name);
        archive(data.pos);
    }

    template<class Archive>
    void
    load(Archive &archive,
         Event_SoundPlay &data) {
        loadHashedString(archive, 0, data.name);
        archive(data.pos);
    }
}
