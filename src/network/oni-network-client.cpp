#include <oni-core/network/oni-network-client.h>

#include <chrono>

#include <enet/enet.h>

#include <oni-core/io/oni-io-output.h>
#include <oni-core/io/oni-io-input.h>
#include <oni-core/util/oni-util-timer.h>
#include <oni-core/network/oni-network-packet.h>
#include <oni-core/entities/oni-entities-serialization.h>

namespace oni {
    namespace network {

        Client::Client() : Peer::Peer(nullptr, 1, 2, 0, 0) {
            mTimer = std::make_unique<utils::Timer>();
        }

        Client::~Client() = default;

        void
        Client::connect(const Address &address) {
            auto enetAddress = ENetAddress{};
            enet_address_set_host(&enetAddress, address.host.c_str());
            enetAddress.port = address.port;

            mEnetServer = enet_host_connect(mEnetHost, &enetAddress, 2, 0);
            if (!mEnetServer) {
                io::printl("Failed to initiate connection to: " + address.host + ":" + std::to_string(address.port));
                assert(false);
            }
            ENetEvent event;
            if (enet_host_service(mEnetHost, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
                io::printl("Connected to: " + address.host + ":" + std::to_string(address.port));
            } else {
                io::printl("Failed connecting to: " + address.host + ":" + std::to_string(address.port));
                assert(false);
            }

            requestSessionSetup();
        }

        void
        Client::disconnect() {
            enet_peer_disconnect_now(mEnetServer, 0);
        }

        void
        Client::pingServer() {
            auto type = PacketType::PING;
            auto data = std::string{};

            send(type, data, mEnetServer);
        }

        void
        Client::handle(ENetPeer *peer,
                       enet_uint8 *data,
                       common::size size,
                       PacketType header) {
            auto peerID = getPeerID(*peer);
            assert(mPacketHandlers.find(header) != mPacketHandlers.end());
            switch (header) {
                case (PacketType::PING): {
                    auto latency = mTimer->elapsedInSeconds();
                    mTimer->restart();
                    mPacketHandlers[PacketType::PING](peerID, std::to_string(latency * 1000));
                    break;
                }
                case (PacketType::MESSAGE): {
                    auto packet = entities::deserialize<Packet_Data>(data, size);
                    std::cout << packet.data << std::endl;
                    break;
                }
                case (PacketType::CAR_ENTITY_ID):
                case (PacketType::REGISTRY_REPLACE_ALL_ENTITIES):
                case (PacketType::REGISTRY_ONLY_COMPONENT_UPDATE):
                case (PacketType::REGISTRY_ADD_NEW_ENTITIES):
                case (PacketType::REGISTRY_DESTROYED_ENTITIES):
                case (PacketType::EVENT_SOUND_PLAY):
                case (PacketType::EVENT_ROCKET_LAUNCH):
                case (PacketType::EVENT_COLLISION): {
                    auto dataString = std::string(reinterpret_cast<char *>(data), size);
                    mPacketHandlers[header](peerID, dataString);
                    break;
                }
                default: {
                    assert(false);
                    break;
                }
            }
        }

        void
        Client::sendMessage(std::string &&message) {
            auto type = PacketType::MESSAGE;
            auto messagePacket = Packet_Data{std::move(message)};
            auto data = entities::serialize<Packet_Data>(messagePacket);

            send(type, data, mEnetServer);
        }

        void
        Client::postConnectHook(const ENetEvent *event) {
        }

        void
        Client::postDisconnectHook(const ENetEvent *event) {

        }

        void
        Client::sendInput(const io::Input *input) {
            if (!input->hasData()) {
                return;
            }
            auto type = PacketType::CLIENT_INPUT;
            auto data = entities::serialize<io::Input>(*input);

            send(type, data, mEnetServer);
        }

        void
        Client::requestSessionSetup() {
            auto type = PacketType::SETUP_SESSION;
            auto data = std::string{};
            send(type, data, mEnetServer);
        }
    }
}