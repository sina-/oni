#pragma once

#include <cassert>

#include <enet/enet.h>

#include <oni-core/common/typedefs.h>
#include <oni-core/network/packet.h>
#include <oni-core/network/packet-operation.h>

struct _ENetAddress;
typedef struct _ENetAddress ENetAddress;

struct _ENetHost;
typedef struct _ENetHost ENetHost;

struct _ENetPeer;
typedef struct _ENetPeer ENetPeer;

struct _ENetPacket;
typedef struct _ENetPacket ENetPacket;

namespace oni {
    namespace network {
        struct Address {
            std::string host;
            common::uint16 port;
        };


        class Peer {
        protected:
            Peer();

            Peer(const Address *address, common::uint8 peerCount,
                 common::uint8 channelLimit,
                 common::uint32 incomingBandwidth,
                 common::uint32 outgoingBandwidth);

        public:
            virtual ~Peer();

            void poll();

        protected:
            virtual void handle(ENetEvent *event) = 0;

            template<class T>
            void sendPacket(const T *packet, ENetPeer *peer) {
                static_assert(std::is_base_of<Packet, T>::value, "T must inherit from Packet");

                auto size = sizeof(*packet);

                const auto *data = serialize(packet);
                send(data, size, peer);
            }

            void send(const common::uint8 *data, size_t size, ENetPeer *peer);

        protected:
            ENetHost *mEnetHost;
        };
    }
}
