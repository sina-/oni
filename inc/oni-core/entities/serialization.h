#pragma once

#include <sstream>

#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include <oni-core/common/typedefs.h>
#include <oni-core/component/snapshot-type.h>
#include <oni-core/network/packet.h>

namespace oni {
    namespace network {
        // NOTE: These functions can't be defined in serialization.cpp since that would make them invisible to users in
        // other compilation units such as server.cpp
        template<class Archive>
        void serialize(Archive &archive, DataPacket &packet) {
            archive(packet.data);
        }

        template<class Archive>
        void serialize(Archive &archive, EntityPacket &packet) {
            archive(packet.entity);
        }

        template<class Archive>
        void serialize(Archive &archive, ZLevelDeltaPacket &packet) {
            archive(packet.major, packet.minor);
        }
    }

    namespace entities {
        class EntityManager;

/*        template<class... Components>
        std::string serialize_(entities::EntityManager &manager) {
            std::stringstream storage;
            {
                cereal::PortableBinaryOutputArchive output{storage};
                manager.snapshot<cereal::PortableBinaryOutputArchive, Components...>(output);
            }

            return storage.str();
        }

        template<class... Component>
        std::string deserialize_(entities::EntityManager &manager, const std::string &data) {
            std::stringstream storage;
            storage.str(data);

            {
                cereal::PortableBinaryInputArchive input{storage};
                manager.restore<cereal::PortableBinaryInputArchive,
                        Component...>
                        (input);
            }
            return storage.str();
        }*/

        std::string serialize(entities::EntityManager &manager, component::SnapshotType snapshotType);

        void deserialize(oni::entities::EntityManager &manager, const std::string &data, component::SnapshotType snapshotType);

        template<class T>
        T deserialize(const std::string &data) {
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
        T deserialize(const common::uint8 *data, size_t size) {
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
        std::string serialize(const T &data) {
            std::ostringstream storage;
            {
                cereal::PortableBinaryOutputArchive output{storage};

                output(data);
            }
            return storage.str();
        }


    }
}
