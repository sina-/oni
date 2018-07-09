#include <oni-core/network/packet.h>

namespace oni {
    namespace network {
        PacketData::PacketData(Packet *packet) : mSize(sizeof(packet)),
                                                       mPacket(packet) {
            mData = reinterpret_cast<void *> (&mPacket);
        }

        PacketData::PacketData(void *data, size_t size) : mData(data), mSize(size) {
            mPacket = reinterpret_cast<Packet*>(data);
        }

        PacketData::~PacketData() {
            mData = nullptr;
        }

        void *PacketData::serialize() const {
            return mData;
        }

        size_t PacketData::getSize() const {
            return mSize;
        }

        PacketType PacketData::getHeader() const {
            return mPacket->getHeader();
        }

        PacketPing::PacketPing(common::uint32 timestamp_) : Packet(PacketType::PING),
                                                            mTimestamp(timestamp_) {}

        common::uint32 PacketPing::getTimeStamp() const {
            return mTimestamp;
        }

        PacketType Packet::getHeader() const {
            return mHeader;
        }
    }
}