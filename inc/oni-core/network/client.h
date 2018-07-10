#pragma once

#include <oni-core/network/peer.h>
#include <enet/enet.h>

namespace oni {
    namespace network {
        class Client : public Peer {
        public:
            Client();

            ~Client() override;

            void connect(const Address &address);

            void pingServer();

            void sendMessage(const std::string &message);

        protected:
            void handle(ENetEvent *event) override;

        protected:
            ENetPeer *mEnetPeer;
        };
    }
}
