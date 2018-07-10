#include <oni-core/network/server.h>

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <assert.h>

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

        void Server::handle(ENetEvent *event) {
            if (!event->packet->data) {
                return;
            }
            if (!event->packet->dataLength) {
                return;
            }

            auto packet = Packet(event->packet->data, event->packet->dataLength);
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

            ENetPacket *packetToClient = enet_packet_create(event->packet->data, event->packet->dataLength,
                                                            ENET_PACKET_FLAG_RELIABLE);

            assert(packetToClient);

            auto success = enet_peer_send(event->peer, 0, packetToClient);

            assert(success == 0);

            enet_host_flush(mEnetHost);
        }

        void Server::handle(const PingPacket &packet) {
            auto time_t = std::time_t{packet.getTimeStamp()};
        }
    }
}
