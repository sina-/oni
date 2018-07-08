#pragma once

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace network {
        enum class PacketType {
            PING,

            LAST
        };

        struct Packet {
            PacketType header;
        };

        struct PacketPing : public Packet {
            common::uint32 timestamp;
        };

        class PacketData {
        public:
            explicit PacketData(const Packet &packet_);

            PacketData(void *data_, size_t size_);

            ~PacketData();

            Packet *deserialize();

            void *serialize();

            size_t getSize();

        private:
            PacketType header{};
            void *data{};
            Packet packet{};
            size_t size{};
        };
    }
}
