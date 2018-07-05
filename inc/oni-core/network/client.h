#pragma once

#include <enet/enet.h>
#include <oni-core/network/server.h>
#include <oni-core/network/packet.h>
#include <oni-core/network/peer.h>

namespace oni {
    namespace network {
        class Client : public Peer {
        public:
            Client();

            ~Client() override;

            void connect(const Address &address);

            void pingServer();

        protected:
            void handle(const common::uint8 *data, ENetPeer *peer) override;

        protected:
            ENetPeer *mEnetPeer;
        };
    }
}
