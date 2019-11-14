#pragma once

#include <sstream>

#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/deque.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/map.hpp>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/component/oni-component-audio.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-visual.h>
#include <oni-core/entities/oni-entities-manager.h>
#include <oni-core/entities/oni-entities-serialization-hashed-string.h>
#include <oni-core/entities/oni-entities-structure.h>
#include <oni-core/game/oni-game-event.h>
#include <oni-core/network/oni-network-packet.h>


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
        saveHashedString(archive, 0, data);
    }

    template<class Archive>
    void
    load(Archive &archive,
         EntityName &data) {
        loadHashedString(archive, 0, data);
    }

    template<class Archive>
    void
    save(Archive &archive,
         const DeletedEntity &data) {
        archive(data.id);
        saveHashedString(archive, 0, data.name);
    }

    template<class Archive>
    void
    load(Archive &archive,
         DeletedEntity &data) {
        archive(data.id);
        loadHashedString(archive, 0, data.name);
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
    save(Archive &archive,
         const PhysicalCategory &data) {
        saveEnum(archive, 0, data);
    }

    template<class Archive>
    void
    load(Archive &archive,
         PhysicalCategory &data) {
        loadEnum(archive, 0, data);
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

    template<class ...Components>
    std::string
    serialize(EntityManager &manager,
              SnapshotType snapshotType) {
        auto storage = std::stringstream{};
        {
            cereal::PortableBinaryOutputArchive output{storage};
            manager.snapshot<cereal::PortableBinaryOutputArchive, Components...>(output, snapshotType);
        }

        return storage.str();
    }

    template<class ...Components, class... Type, class... Member>
    void
    deserialize(oni::EntityManager &manager,
                const std::string &data,
                SnapshotType snapshotType,
                Member Type::*... member) {
        auto storage = std::stringstream{};
        storage.str(data);
        {
            cereal::PortableBinaryInputArchive input{storage};
            manager.restore<cereal::PortableBinaryInputArchive, Components...>(snapshotType, input, member...);
        }
    }

    template<class T>
    T
    deserialize(const std::string &data) {
        std::istringstream storage;
        storage.str(data);

        T result;
        {
            cereal::PortableBinaryInputArchive input{storage};
            input(result);
        }
        return result;
    }

    template<class T>
    T
    deserialize(const u8 *data,
                size_t size) {
        std::istringstream storage;
        storage.str(std::string(reinterpret_cast<const char *>(data), size));

        T result;
        {
            cereal::PortableBinaryInputArchive input{storage};
            input(result);
        }
        return result;
    }


    template<class T>
    std::string
    serialize(const T &data) {
        std::ostringstream storage;
        {
            cereal::PortableBinaryOutputArchive output{storage};

            output(data);
        }
        return storage.str();
    }
}
