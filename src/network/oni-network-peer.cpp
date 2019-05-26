#include <oni-core/network/oni-network-peer.h>

#include <enet/enet.h>

#include <cstring>

namespace oni {
    namespace network {
        Peer::Peer() = default;

        Peer::Peer(const Address *address,
                   common::u8 peerCount,
                   common::u8 channelLimit,
                   common::u32 incomingBandwidth,
                   common::u32 outgoingBandwidth) {

            auto result = enet_initialize();
            if (result) {
                throw std::runtime_error("An error occurred while initializing server.\n");
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
            assert(mEnetHost);

            // TODO: Compressor doesnt seem to reduce the bit-rate :/
//            result = enet_host_compress_with_range_coder(mEnetHost);
//            assert(result >= 0);
        }

        Peer::~Peer() {
            enet_host_destroy(mEnetHost);
            enet_deinitialize();
        }

        void
        Peer::flush() {
            enet_host_flush(mEnetHost);

            auto elapsed = mUploadTimer.elapsedInSeconds();
            if (elapsed >= 1.0f) {
                mUploadBPS = mTotalUpload / elapsed;
                mTotalUpload = 0;
                mUploadTimer.restart();
            }
        }

        void
        Peer::poll() {
            ENetEvent event;

            while (enet_host_service(mEnetHost, &event, 0) > 0) {

                char ip[16]{};
                enet_address_get_host_ip(&event.peer->address, ip, 16);

                switch (event.type) {
                    case ENET_EVENT_TYPE_CONNECT: {
                        const auto &peedID = getPeerID(*event.peer);
                        printf("A new client connected from %s:%u with client ID: %s.\n", ip, event.peer->address.port,
                               peedID.c_str());
                        mPeers[peedID] = event.peer;

                        postConnectHook(&event);
                        break;
                    }
                    case ENET_EVENT_TYPE_RECEIVE: {
                        // TODO: Need to gather stats on invalid packets and their sources!
                        if (!event.packet->data) {
                            return;
                        }
                        if (!event.packet->dataLength) {
                            return;
                        }

                        mTotalDownload += event.packet->dataLength;

                        auto data = event.packet->data;
                        auto header = getHeader(data);
                        auto dataHeadless = event.packet->dataLength - 1;
                        data += 1;

                        handle(event.peer, data, dataHeadless, header);

                        enet_packet_destroy(event.packet);
                        break;
                    }
                    case ENET_EVENT_TYPE_DISCONNECT: {
                        const auto &peerID = getPeerID(*event.peer);
                        printf("%s:%u disconnected. Client ID: %s \n", ip, event.peer->address.port, peerID.c_str());
                        postDisconnectHook(&event);

                        mPeers.erase(peerID);
                        event.peer->data = nullptr;

                        enet_peer_reset(event.peer);
                        break;
                    }
                    case ENET_EVENT_TYPE_NONE: {
                        break;
                    }
                }
            }

            auto elapsed = mDownloadTimer.elapsedInSeconds();
            if (elapsed >= 1.0f) {
                mDownloadBPS = mTotalDownload / elapsed;
                mTotalDownload = 0;
                mDownloadTimer.restart();
            }
        }

        void
        Peer::send(const common::u8 *data,
                   common::size size,
                   ENetPeer *peer) {
            ENetPacket *packetToServer = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE |
                                                                        ENET_PACKET_FLAG_NO_ALLOCATE);
            assert(packetToServer);
            auto success = enet_peer_send(peer, 0, packetToServer);
            assert(success == 0);

            mTotalUpload += size;
        }

        void
        Peer::send(PacketType type,
                   std::string &data,
                   ENetPeer *peer) {
            data.insert(0, 1, static_cast<std::underlying_type<PacketType>::type>(type));

            ENetPacket *packetToServer = enet_packet_create(data.c_str(), data.size(), ENET_PACKET_FLAG_RELIABLE);
            assert(packetToServer);
            auto success = enet_peer_send(peer, 0, packetToServer);
            assert(success == 0);

            mTotalUpload += data.size();
        }

        PacketType
        Peer::getHeader(const common::u8 *data) const {
            if (!data) {
                return PacketType::UNKNOWN;
            }

            auto header = static_cast<PacketType>(*data);
            return header;
        }

        void
        Peer::broadcast(PacketType type,
                        std::string &data) {
            data.insert(0, 1, static_cast<std::underlying_type<PacketType>::type>(type));
            ENetPacket *packetToPeers = enet_packet_create(data.c_str(), data.size(), ENET_PACKET_FLAG_RELIABLE);
            assert(packetToPeers);
            enet_host_broadcast(mEnetHost, 0, packetToPeers);

            mTotalUpload += data.size();
        }

        void
        Peer::registerPacketHandler(PacketType type,
                                    std::function<
                                            void(const common::PeerID &,
                                                 const std::string &)> &&handler) {
            assert(mPacketHandlers.find(type) == mPacketHandlers.end());
            mPacketHandlers[type] = std::move(handler);
        }

        void
        Peer::registerPostDisconnectHook(std::function<void(const common::PeerID &)> &&handler) {
            mPostDisconnectHook = std::move(handler);
        }

        common::PeerID
        Peer::getPeerID(const ENetPeer &peer) const {
            return std::to_string(peer.connectID);
        }

        common::r32
        Peer::getDownloadKBPS() const {
            return mDownloadBPS / (1000 * 1);
        }

        common::r32
        Peer::getUploadKBPS() const {
            return mUploadBPS / (1000 * 1);
        }
    }
}