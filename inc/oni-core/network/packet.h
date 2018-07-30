#pragma once

#include <cereal/types/base_class.hpp>

#include <oni-core/common/typedefs.h>
#include <oni-core/network/packet-types.h>


namespace oni {
    namespace network {

        struct PingPacket {
            common::uint64 timestamp{0};

            template<class Archive>
            void serialize(Archive &archive) {
                archive(timestamp);
            }
        };

        struct EntityPacket {
            common::uint32 entity{};

            template<class Archive>
            void serialize(Archive &archive) {
                archive(entity);
            }
        };

        struct DataPacket {
            std::string data{};

            template<class Archive>
            void serialize(Archive &archive) {
                archive(data);
            }
        };

    }
}
