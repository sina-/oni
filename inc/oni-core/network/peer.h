#pragma once

#include <oni-core/common/typedefs.h>

struct _ENetAddress;
typedef struct _ENetAddress ENetAddress;

struct _ENetHost;
typedef struct _ENetHost ENetHost;

struct _ENetPeer;
typedef struct _ENetPeer ENetPeer;

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
            virtual void handle(const common::uint8 *data, ENetPeer *peer) = 0;

        protected:
            ENetHost *mEnetHost;
        };
    }
}
