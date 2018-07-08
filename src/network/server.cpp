#include <oni-core/network/server.h>

#include <stdexcept>
#include <iostream>

#include <enet/enet.h>

#include <oni-core/network/packet.h>

namespace oni {
    namespace network {
        Server::Server() = default;

        Server::~Server() = default;

        Server::Server(const Address *address, common::uint8 numClients, common::uint8 numChannels) :
                Peer::Peer(address, numClients, numChannels, 0, 0) {
        }

        void Server::handle(const ENetPacket *packet, ENetPeer *peer) {
            auto gamePacket = PacketData(packet->data, packet->dataLength).deserialize();

            // TODO: WTF?
            auto pingPacket = static_cast<PacketPing *>(gamePacket);
            handle(*pingPacket);

            ENetPacket *eNetPacket = enet_packet_create(packet->data, packet->dataLength, ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peer, 0, eNetPacket);

            enet_host_flush(mEnetHost);
        }

        void Server::handle(const PacketPing &packet) {
            std::cout << packet.timestamp << std::endl;
        }
    }
}
