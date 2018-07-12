#pragma once

#include <oni-core/network/packet.h>

namespace oni {
    namespace network {
        template<class T>
        const T *deserialize(const common::uint8 *data) {
            static_assert(std::is_base_of<Packet, T>::value, "T must inherit from Packet");

            const auto *packet = reinterpret_cast<const T *>(data);
            return packet;
        }

        const common::uint8 *serialize(const Packet *packet);

        PacketType getHeader(const common::uint8 *data);
    }
}
