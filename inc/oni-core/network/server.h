#pragma once

#include <oni-core/common/typedefs.h>
#include <oni-core/network/peer.h>
#include <oni-core/network/packet.h>
#include <enet/enet.h>

namespace oni {
    namespace network {
        class Server : public Peer {
        public:
            Server(const Address *address, common::uint8 numClients, common::uint8 numChannels);

            void sendWorldData(std::string &&data) const;

            ~Server() override;

        private:
            Server();

            void handle(ENetEvent *event) override;
        };
    }
}
