#pragma once

#include <enet/enet.h>

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace network {

        struct Address {
            std::string host;
            common::uint16 port;
        };

        class Server {
        public:
            Server(const Address &address, common::uint8 numClients, common::uint8 numChannels);

            void poll();

            ~Server();

        private:
            Server();

            ENetHost *mServer;
        };
    }
}
