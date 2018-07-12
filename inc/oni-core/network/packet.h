#pragma once

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace network {
        enum class PacketType {
            PING,
            MESSAGE,

            UNKNOWN
        };

        class Packet {
        public:
            explicit Packet(PacketType header);

            PacketType getHeader() const;

        private:
            PacketType mHeader;
        };

        class PingPacket : public Packet {
        public:
            explicit PingPacket(common::uint64 timestamp);

            common::uint64 getTimeStamp() const;

        private:
            common::uint64 mTimestamp{};
        };

        class MessagePacket : public Packet {
        public:
            explicit MessagePacket(const std::string &message);

            std::string getMessage() const;

        private:
            // TODO: replace this with array<common::uint8>[32] and introduce utility function to split-up
            // longer text into several packets and just iterate over them when sending the message
            std::string mMessage{};
        };

    }
}
