#pragma once

#include <oni-core/common/typedefs.h>
#include <oni-core/network/peer.h>

namespace oni {
    namespace network {
        class Server : public Peer {
        public:
            Server(const Address *address, common::uint8 numClients, common::uint8 numChannels);

            ~Server() override;

        private:
            Server();

            void handle(const common::uint8 *data, ENetPeer *peer) override;
        };
    }
}
