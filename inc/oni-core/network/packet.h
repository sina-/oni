#pragma once

#include <cstddef>

#include <oni-core/network/game-packet.h>

namespace oni {
    namespace network {

        class Packet {
        public:
            explicit Packet(GamePacket *gamePacket, size_t size);

            Packet(common::uint8 *data, std::size_t size);

            ~Packet();

            template<class T>
            T *deserialize() {
                static_assert(std::is_base_of<GamePacket, T>::value, "T must inherit from GamePacket");

                auto *packet = reinterpret_cast<T *>(mData);
                return packet;
            }

            void *serialize() const;

            size_t getSize() const;

            PacketType getHeader() const;

        private:
            PacketType mHeader{PacketType::UNKNOWN};
            size_t mSize{};
            void *mData{};
        };
    }
}
