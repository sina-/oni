#include <oni-core/network/client.h>

#include <string>

#include <oni-core/io/output.h>

namespace oni {
    namespace network {

        Client::Client() {
            mEnetClient = enet_host_create(nullptr, 1, 2, 0, 0);
            if (!mEnetClient) {
                std::runtime_error("An error occurred while creating client.\n");
            }
        }

        Client::~Client() {
            enet_host_destroy(mEnetClient);
        }

        void Client::connect(const Address &address) {
            auto enetAddress = ENetAddress{};
            enet_address_set_host(&enetAddress, address.host.c_str());
            enetAddress.port = address.port;

            mEnetServer = enet_host_connect(mEnetClient, &enetAddress, 2, 0);
            if (!mEnetServer) {
                std::runtime_error(
                        "Failed to initiate connection to: " + address.host + ":" + std::to_string(address.port));
            }
            ENetEvent event;
            if (enet_host_service(mEnetClient, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
                io::printl("Connected to: " + address.host + ":" + std::to_string(address.port));
            } else {
                std::runtime_error(
                        "Failed connecting to: " + address.host + ":" + std::to_string(address.port));
            }
        }

        void Client::pingServer() {

            std::string data{"ping"};
            ENetPacket *packet = enet_packet_create(data.data(), data.size() + 1, ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(mEnetServer, 0, packet);

            enet_host_flush(mEnetClient);
        }

        void Client::poll() {
            ENetEvent event;
            while (enet_host_service(mEnetClient, &event, 0) > 0) {

                char ip[16]{};
                enet_address_get_host_ip(&event.peer->address, ip, 16);

                switch (event.type) {
                    case ENET_EVENT_TYPE_CONNECT: {
                        printf("A new client connected from %x:%u.\n",
                               event.peer->address.host,
                               event.peer->address.port);
                        break;
                    }
                    case ENET_EVENT_TYPE_RECEIVE: {
                        printf("A packet of length %u containing %s was received from %s on channel %u.\n",
                               static_cast<unsigned int>(event.packet->dataLength),
                               event.packet->data,
                               ip,
                               event.channelID);

                        handle(event.packet->data, event.peer);

                        enet_packet_destroy(event.packet);
                        break;
                    }
                    case ENET_EVENT_TYPE_DISCONNECT: {
                        printf("%s disconnected.\n", ip);
                        event.peer->data = nullptr;
                    }
                    case ENET_EVENT_TYPE_NONE: {
                        break;
                    }
                }
            }
        }

        void Client::handle(const common::uint8 *data, ENetPeer *peer) {
            io::printl("Received pong from server!");
        }

    }
}