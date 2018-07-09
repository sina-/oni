#include <oni-core/network/client.h>

#include <string>
#include <chrono>
#include <iostream>

#include <enet/enet.h>

#include <oni-core/io/output.h>
#include <oni-core/network/game-packet.h>
#include <oni-core/network/packet.h>

namespace oni {
    namespace network {

        Client::Client() : Peer::Peer(nullptr, 1, 2, 0, 0) {
        }

        Client::~Client() = default;

        void Client::connect(const Address &address) {
            auto enetAddress = ENetAddress{};
            enet_address_set_host(&enetAddress, address.host.c_str());
            enetAddress.port = address.port;

            mEnetPeer = enet_host_connect(mEnetHost, &enetAddress, 2, 0);
            if (!mEnetPeer) {
                std::runtime_error(
                        "Failed to initiate connection to: " + address.host + ":" + std::to_string(address.port));
            }
            ENetEvent event;
            if (enet_host_service(mEnetHost, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
                io::printl("Connected to: " + address.host + ":" + std::to_string(address.port));
            } else {
                std::runtime_error(
                        "Failed connecting to: " + address.host + ":" + std::to_string(address.port));
            }
        }

        void Client::pingServer() {
            auto now = std::chrono::system_clock::now().time_since_epoch().count();
            auto ping = PingPacket{static_cast<common::uint64>(now)};

            auto pingPacket = Packet(&ping);

            ENetPacket *packet = enet_packet_create(pingPacket.serialize(), pingPacket.getSize(),
                                                    ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(mEnetPeer, 0, packet);

            enet_host_flush(mEnetHost);
        }

        void Client::handle(const ENetPacket *packet, ENetPeer *peer) {
        }

    }
}