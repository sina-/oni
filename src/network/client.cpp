#include <oni-core/network/client.h>

#include <chrono>

#include <enet/enet.h>

#include <oni-core/io/output.h>
#include <oni-core/io/input.h>
#include <oni-core/utils/timer.h>
#include <oni-core/entities/serialization.h>

namespace oni {
    namespace network {

        Client::Client() : Peer::Peer(nullptr, 1, 2, 0, 0) {
            mTimer = std::make_unique<utils::Timer>();
        }

        Client::~Client() = default;

        void Client::connect(const Address &address) {
            auto enetAddress = ENetAddress{};
            enet_address_set_host(&enetAddress, address.host.c_str());
            enetAddress.port = address.port;

            mEnetPeer = enet_host_connect(mEnetHost, &enetAddress, 2, 0);
            if (!mEnetPeer) {
                throw std::runtime_error(
                        "Failed to initiate connection to: " + address.host + ":" + std::to_string(address.port));
            }
            ENetEvent event;
            if (enet_host_service(mEnetHost, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
                io::printl("Connected to: " + address.host + ":" + std::to_string(address.port));
            } else {
                throw std::runtime_error(
                        "Failed connecting to: " + address.host + ":" + std::to_string(address.port));
            }

            requestSessionSetup();
        }

        void Client::pingServer() {
            auto type = PacketType::PING;
            auto data = std::string{};

            send(type, data, mEnetPeer);
        }

        void Client::handle(ENetPeer *peer, enet_uint8 *data, size_t size, PacketType header) {
            auto peerID = getPeerID(peer->address);
            switch (header) {
                case (PacketType::PING): {
                    auto latency = mTimer->elapsed_in_seconds();
                    mTimer->restart();
                    mPacketHandlers[PacketType::PING](peerID, std::to_string(latency * 1000));
                    break;
                }
                case (PacketType::MESSAGE): {
                    auto packet = entities::deserialize<DataPacket>(data, size);
                    std::cout << packet.data << std::endl;
                    break;
                }
                case (PacketType::CAR_ENTITY_ID): {
                    auto dataString = std::string(reinterpret_cast<char *>(data), size);
                    mPacketHandlers[PacketType::CAR_ENTITY_ID](peerID, dataString);
                    break;
                }
                case (PacketType::REPLACE_ALL_ENTITIES): {
                    auto dataString = std::string(reinterpret_cast<char *>(data), size);
                    mPacketHandlers[PacketType::REPLACE_ALL_ENTITIES](peerID, dataString);
                    break;
                }
                case (PacketType::ONLY_COMPONENT_UPDATE): {
                    auto dataString = std::string(reinterpret_cast<char *>(data), size);
                    mPacketHandlers[PacketType::ONLY_COMPONENT_UPDATE](peerID, dataString);
                    break;
                }
                case (PacketType::ADD_NEW_ENTITIES): {
                    auto dataString = std::string(reinterpret_cast<char *>(data), size);
                    mPacketHandlers[PacketType::ADD_NEW_ENTITIES](peerID, dataString);
                    break;
                }
                case (PacketType::DESTROYED_ENTITIES): {
                    auto dataString = std::string(reinterpret_cast<char *>(data), size);
                    mPacketHandlers[PacketType::DESTROYED_ENTITIES](peerID, dataString);
                    break;
                }
                default: {
                    std::cout << "Unknown packet!" << std::endl;
                    break;
                }
            }
        }

        void Client::sendMessage(std::string &&message) {
            auto type = PacketType::MESSAGE;
            auto messagePacket = DataPacket{std::move(message)};
            auto data = entities::serialize<DataPacket>(messagePacket);

            send(type, data, mEnetPeer);
        }

        void Client::postConnectHook(const ENetEvent *event) {
        }

        void Client::postDisconnectHook(const ENetEvent *event) {

        }

        void Client::sendInput(const io::Input *input) {
            auto type = PacketType::CLIENT_INPUT;
            auto data = entities::serialize<io::Input>(*input);

            send(type, data, mEnetPeer);
        }

        void Client::requestSessionSetup() {
            auto type = PacketType::SETUP_SESSION;
            auto data = std::string{};
            send(type, data, mEnetPeer);
        }
    }
}