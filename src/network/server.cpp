#include <oni-core/network/server.h>

#include <stdexcept>
#include <iostream>
#include <iomanip>

#include <enet/enet.h>

#include <oni-core/network/game-packet.h>
#include <oni-core/network/packet.h>

namespace oni {
    namespace network {
        Server::Server() = default;

        Server::~Server() = default;

        Server::Server(const Address *address, common::uint8 numClients, common::uint8 numChannels) :
                Peer::Peer(address, numClients, numChannels, 0, 0) {
        }

        void Server::handle(const ENetPacket *eNetPacket, ENetPeer *peer) {
            auto packet = Packet(eNetPacket->data, eNetPacket->dataLength);
            switch (packet.getHeader()) {
                case (PacketType::PING): {
                    auto pingPacket = packet.deserialize<PingPacket>();
                    std::cout << pingPacket->getTimeStamp() << std::endl;
                    handle(*pingPacket);
                    break;
                }
                default: {
                    break;
                }
            }

            ENetPacket *eNetPacketToClient = enet_packet_create(eNetPacket->data, eNetPacket->dataLength,
                                                                ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peer, 0, eNetPacketToClient);

            enet_host_flush(mEnetHost);
        }

        void Server::handle(const PingPacket &packet) {
            auto time_t = std::time_t{packet.getTimeStamp()};
        }
    }
}
