#include <oni-core/network/server.h>

#include <stdexcept>


namespace oni {
    namespace network {
        Server::Server() = default;

        Server::~Server() {
            enet_host_destroy(mServer);
            enet_deinitialize();
        }

        Server::Server(const Address &address, common::uint8 numClients, common::uint8 numChannels) {
            auto result = enet_initialize();
            if (result != 0) {
                std::runtime_error("An error occurred while initializing server.\n");
            }

            auto enetAddress = ENetAddress{};
            enet_address_set_host(&enetAddress, address.host.c_str());
            enetAddress.port = address.port;

            mServer = enet_host_create(&enetAddress, numClients, numChannels, 0, 0);
            if (mServer == nullptr) {
                std::runtime_error("An error occurred while creating server.\n");
            }
        }

        void Server::poll() {
            ENetEvent event;
            while (enet_host_service(mServer, &event, 0) > 0) {
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
                               static_cast<char *>(event.peer->data),
                               event.channelID);
                        enet_packet_destroy(event.packet);
                        break;
                    }
                    case ENET_EVENT_TYPE_DISCONNECT: {
                        printf("%s disconnected.\n", static_cast<char *>(event.peer->data));
                        event.peer->data = nullptr;
                    }
                    case ENET_EVENT_TYPE_NONE: {
                        break;
                    }
                }
            }
        }
    }
}
