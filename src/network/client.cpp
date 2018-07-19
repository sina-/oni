#include <oni-core/network/client.h>

#include <string>
#include <chrono>
#include <iostream>

#include <enet/enet.h>

#include <oni-core/io/output.h>
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
            auto type = PacketType::PING;
            auto pingPacket = PingPacket{now};
            auto data = serialize<PingPacket>(pingPacket);

            send(type, data, mEnetPeer);
        }

        void Client::handle(ENetEvent *event) {
            // TODO: Refactor server handle function to share parts of the logic that has to do with figuring out
            // header and reading the data. Client and server will just write logic to handle specific type of
            // packets: handlePing(), handleMessage()... these functions can be virtual in peer
        }

        void Client::sendMessage(const std::string &message) {
            auto type = PacketType::MESSAGE;
            auto messagePacket = MessagePacket{message};
            auto data = serialize<MessagePacket>(messagePacket);

            send(type, data, mEnetPeer);
        }

        void Client::sendEntities(const std::string &data) {
            auto type = PacketType::WORLD_DATA;
            send(type, data, mEnetPeer);
        }
    }
}