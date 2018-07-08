#pragma once

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
            void handle(const ENetPacket *packet, ENetPeer *peer) override;

        protected:
            ENetPeer *mEnetPeer;
        };
    }
}
