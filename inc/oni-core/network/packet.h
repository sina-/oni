#pragma once

#include <oni-core/common/typedefs.h>
#include <oni-core/network/packet-types.h>


namespace oni {
    namespace network {

        struct PingPacket {
            template<class Archive>
            void serialize(Archive &archive) {}
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
