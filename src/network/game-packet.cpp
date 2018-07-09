#include <oni-core/network/game-packet.h>

namespace oni {
    namespace network {

        GamePacket::GamePacket(PacketType header) : mHeader(header) {}

        PingPacket::PingPacket(common::uint64 timestamp) : GamePacket(PacketType::PING),
                                                           mTimestamp(timestamp) {}

        common::uint64 PingPacket::getTimeStamp() const {
            return mTimestamp;
        }

        PacketType GamePacket::getHeader() const {
            return mHeader;
        }

    }
}