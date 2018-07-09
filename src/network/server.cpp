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
            // TODO: PacketData needs to create a copy of the packet->data instead of just keeping
            // a reference to it. As it is now, PacketData's life-time is tied to ENetPacket *packet.
            auto gamePacket = PacketData(packet->data, packet->dataLength);
            switch (gamePacket.getHeader()) {
                case (PacketType::PING): {
                    auto pingPacket = gamePacket.deserialize<PacketPing>();
                    handle(*pingPacket);
                    break;
                }
                default: {
                    break;
                }
            }

            ENetPacket *eNetPacket = enet_packet_create(packet->data, packet->dataLength, ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peer, 0, eNetPacket);

            enet_host_flush(mEnetHost);
        }

        void Server::handle(const PacketPing &packet) {
            std::cout << packet.getTimeStamp() << std::endl;
        }
    }
}
