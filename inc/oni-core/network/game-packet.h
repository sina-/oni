#pragma once

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace network {
        enum class PacketType {
            PING,
            MESSAGE,

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

        class MessagePacket : public GamePacket {
        public:
            explicit MessagePacket(const std::string &message);

            std::string getMessage() const;
        private:
            std::string mMessage{};
        };

    }
}
