#include <oni-core/network/packet.h>

namespace oni {
    namespace network {
        PacketData::PacketData(const Packet &packet_) : header(packet.header), size(sizeof(packet_)),
                                                        packet(packet_) {
            data = reinterpret_cast<void *> (&packet);
        }

        PacketData::PacketData(void *data_, size_t size_) : data(data_), size(size) {
            deserialize();
        }

        PacketData::~PacketData() {

            data = nullptr;
        }

        Packet *PacketData::deserialize() {
            Packet *packet{};

            switch (header) {
                case PacketType::PING: {
                    packet = reinterpret_cast<PacketPing *>(data);
                }
                default: {
                    break;
                }
            }
            header = packet->header;
            return packet;
        }

        void *PacketData::serialize() {
            return data;
        }

        size_t PacketData::getSize() {
            return size;
        }
    }
}