#include <oni-core/network/packet.h>

namespace oni {
    namespace network {

        Packet::Packet(PacketType header) : mHeader(header) {}

        PingPacket::PingPacket(common::uint64 timestamp) : Packet(PacketType::PING),
                                                           mTimestamp(timestamp) {}

        common::uint64 PingPacket::getTimeStamp() const {
            return mTimestamp;
        }

        PacketType Packet::getHeader() const {
            return mHeader;
        }

        MessagePacket::MessagePacket(const std::string &message) : Packet(PacketType::MESSAGE), mMessage(message) {}

        std::string MessagePacket::getMessage() const {
            return mMessage;
        }
    }
}