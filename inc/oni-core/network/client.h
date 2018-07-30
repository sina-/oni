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

            void sendMessage(std::string &&message);

        private:
            void handle(ENetPeer *peer, enet_uint8 *data, size_t size, PacketType header) override;

            void postConnectHook(const ENetEvent *event) override;

            void postDisconnectHook(const ENetEvent *event) override;

        private:
            ENetPeer *mEnetPeer;
        };
    }
}
