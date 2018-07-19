#pragma once

#include <cereal/types/base_class.hpp>

#include <oni-core/common/typedefs.h>
#include <oni-core/network/packet-types.h>

namespace oni {
    namespace network {

/*        struct Packet {
            PacketType header{PacketType::UNKNOWN};

            template<class Archive>
            void serialize(Archive &archive) {
                archive(header);
            }
        };*/

        struct PingPacket /*: public Packet */{
/*            explicit PingPacket(common::uint64 timestamp_) : header(PacketType::PING), timestamp(timestamp_) {}*/

            common::uint64 timestamp{};

            template<class Archive>
            void serialize(Archive &archive) {
                archive(/*cereal::base_class<Packet>(this),*/
                        timestamp);
            }
        };

        struct MessagePacket /*: public Packet*/ {
            std::string message{};

            template<class Archive>
            void serialize(Archive &archive) {
                archive(/*cereal::base_class<Packet>(this),*/
                        message);
            }
        };

    }
}
