#include <oni-core/network/packet.h>

#include <cstring>

namespace oni {
    namespace network {
        Packet::Packet(GamePacket *gamePacket, size_t size) : mSize(size),
                                                              mHeader(gamePacket->getHeader()) {
            mData = new common::uint8[mSize];
            memcpy(mData, (common::uint8 *) (gamePacket), mSize);
        }

        Packet::Packet(common::uint8 *data, size_t size) : mSize(size) {
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