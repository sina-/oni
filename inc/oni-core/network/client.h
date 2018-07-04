#pragma once

#include <enet/enet.h>
#include <oni-core/network/server.h>
#include <oni-core/network/packet.h>

namespace oni {
    namespace network {
        class Client {
        public:
            Client();

            ~Client();

            void connect(const Address &address);

            void pingServer();

            void poll();

        private:
            void handle(const common::uint8 *data, ENetPeer *peer);

        private:
            ENetHost *mEnetClient;
            ENetPeer *mEnetServer;
        };
    }
}
