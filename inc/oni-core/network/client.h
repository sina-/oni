#pragma once

#include <enet/enet.h>

#include <oni-core/network/peer.h>

namespace oni {
    namespace network {
        class Client : public Peer {
        public:
            Client();

            ~Client() override;

            void connect(const Address &address);

            void pingServer();

            void sendMessage(const std::string &message);

            void sendEntities(const std::string &data);

        protected:
            void handle(ENetEvent *event) override;

        protected:
            ENetPeer *mEnetPeer;
        };
    }
}
