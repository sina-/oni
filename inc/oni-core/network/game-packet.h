#pragma once

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace network {
        enum class PacketType {
            PING,

            UNKNOWN
        };

        class GamePacket {
        public:
            explicit GamePacket(PacketType header);

            PacketType getHeader() const;

        private:
            PacketType mHeader;
        };

        class PingPacket : public GamePacket {
        public:
            explicit PingPacket(common::uint64 timestamp);

            common::uint64 getTimeStamp() const;

        private:
            common::uint64 mTimestamp{};
        };

    }
}
