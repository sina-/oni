#include <oni-core/network/packet.h>

#include <cstring>

namespace oni {
    namespace network {
        Packet::Packet(GamePacket *gamePacket) : mSize(sizeof(gamePacket)),
                                                       mHeader(gamePacket->getHeader()) {
            mData = new common::uint8[mSize];
            memcpy(mData, reinterpret_cast<void *> (gamePacket), mSize);
        }

        Packet::Packet(void *data, size_t size) : mSize(size) {
            mData = new common::uint8[mSize];
            memcpy(mData, data, mSize);

            if (data && size) {
                mHeader = *reinterpret_cast<PacketType *>(data);
            }
        }

        Packet::~Packet() {
            delete mData;
            mData = nullptr;
        }

        void *Packet::serialize() const {
            return mData;
        }

        size_t Packet::getSize() const {
            return mSize;
        }

        PacketType Packet::getHeader() const {
            return mHeader;
        }
    }
}