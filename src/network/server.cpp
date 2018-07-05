#include <oni-core/network/server.h>

#include <stdexcept>
#include <enet/enet.h>

namespace oni {
    namespace network {
        Server::Server() = default;

        Server::~Server() = default;

        Server::Server(const Address *address, common::uint8 numClients, common::uint8 numChannels) :
                Peer::Peer(address, numClients, numChannels, 0, 0) {
        }

        void Server::handle(const common::uint8 *data, ENetPeer *peer) {
            std::string ping{"ping"};
            ENetPacket *packet = enet_packet_create(ping.data(), ping.size() + 1, ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peer, 0, packet);

            enet_host_flush(mEnetHost);
        }
    }
}
