#include <oni-core/network/server.h>

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <cassert>

#include <enet/enet.h>

#include <oni-core/network/packet.h>
#include <oni-core/network/packet-operation.h>

namespace oni {
    namespace network {
        Server::Server() = default;

        Server::~Server() = default;

        Server::Server(const Address *address, common::uint8 numClients, common::uint8 numChannels) :
                Peer::Peer(address, numClients, numChannels, 0, 0) {
        }

        void Server::handle(ENetEvent *event) {
            // TODO: Need to gather stats on invalid packets and there source!
            if (!event->packet->data) {
                return;
            }
            if (!event->packet->dataLength) {
                return;
            }

            auto data = event->packet->data;
            auto header = getHeader(data);
            switch (header) {
                case (PacketType::PING): {
                    auto packet = deserialize<PingPacket>(data);
                    handle(*packet);
                    break;
                }
                case (PacketType::MESSAGE): {
                    auto packet = deserialize<MessagePacket>(data);
                    handle(*packet);
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
            std::cout << time_t << std::endl;
        }

        void Server::handle(const MessagePacket &packet) {
            // TODO: This segfaults
            //std::cout << packet.getMessage() << std::endl;
        }
    }
}
