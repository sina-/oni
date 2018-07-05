#include <oni-core/network/peer.h>

#include <enet/enet.h>

namespace oni {
    namespace network {
        Peer::Peer() = default;

        Peer::Peer(const Address *address, common::uint8 peerCount,
                   common::uint8 channelLimit,
                   common::uint32 incomingBandwidth,
                   common::uint32 outgoingBandwidth) {

            auto result = enet_initialize();
            if (result) {
                std::runtime_error("An error occurred while initializing server.\n");
            }
            if (address) {
                auto enetAddress = ENetAddress{};
                enet_address_set_host(&enetAddress, address->host.c_str());
                enetAddress.port = address->port;
                mEnetHost = enet_host_create(&enetAddress, peerCount, channelLimit, incomingBandwidth,
                                             outgoingBandwidth);
            } else {
                mEnetHost = enet_host_create(nullptr, peerCount, channelLimit, incomingBandwidth, outgoingBandwidth);
            }

        }

        Peer::~Peer() {
            enet_host_destroy(mEnetHost);
            enet_deinitialize();
        }

        void Peer::poll() {
            ENetEvent event;

            while (enet_host_service(mEnetHost, &event, 0) > 0) {

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
    }
}