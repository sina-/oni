#include <oni-core/network/packet-operation.h>

namespace oni {
    namespace network {

        PacketType getHeader(const common::uint8 *data) {
            auto header = PacketType::UNKNOWN;
            if (data) {
                header = reinterpret_cast<const Packet *>(data)->getHeader();
            }
            return header;
        }

        const common::uint8 *serialize(const Packet *packet) {
            return reinterpret_cast<const common::uint8 *>(packet);
        }
    }
}