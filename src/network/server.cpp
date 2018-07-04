#include <oni-core/network/server.h>

#include <stdexcept>


namespace oni {
    namespace network {
        Server::Server() = default;

        Server::~Server() {
            enet_host_destroy(mEnetServer);
            enet_deinitialize();
        }

        Server::Server(const Address &address, common::uint8 numClients, common::uint8 numChannels) {
            auto result = enet_initialize();
            if (result) {
                std::runtime_error("An error occurred while initializing server.\n");
            }


            auto enetAddress = ENetAddress{};
            enet_address_set_host(&enetAddress, address.host.c_str());
            enetAddress.port = address.port;

            mEnetServer = enet_host_create(&enetAddress, numClients, numChannels, 0, 0);
            if (!mEnetServer) {
                std::runtime_error("An error occurred while creating server.\n");
            }
        }

        void Server::poll() {
            ENetEvent event;

            while (enet_host_service(mEnetServer, &event, 0) > 0) {

                char ip[16]{};
                enet_address_get_host_ip(&event.peer->address, ip, 16);

                switch (event.type) {
                    case ENET_EVENT_TYPE_CONNECT: {
                        printf("A new client connected from %s:%u.\n",
                               ip,
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

        void Server::handle(const common::uint8 *data, ENetPeer *peer) {
            std::string ping{"ping"};
            ENetPacket *packet = enet_packet_create(ping.data(), ping.size() + 1, ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peer, 0, packet);

            enet_host_flush(mEnetServer);
        }
    }
}
