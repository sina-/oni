#pragma once

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace network {
        enum class PacketType {
            PING,

            UNKNOWN
        };

        class Packet {
        public:
            explicit Packet(PacketType header_) : mHeader(header_) {}

            PacketType getHeader() const;

        private:
            PacketType mHeader;
        };

        class PacketPing : public Packet {
        public:
            explicit PacketPing(common::uint32 timestamp_);

            common::uint32 getTimeStamp() const;

        private:
            common::uint32 mTimestamp{};
        };

        class PacketData {
        public:
            explicit PacketData(Packet *packet);

            PacketData(void *data, size_t size);

            ~PacketData();

            template<class T>
            T *deserialize() {
                static_assert(std::is_base_of<Packet, T>::value, "T must inherit from Packet");

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
