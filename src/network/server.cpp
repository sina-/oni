#include <oni-core/network/server.h>

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <cassert>

#include <enet/enet.h>
#include <entt/entity/registry.hpp>
#include <cereal/archives/portable_binary.hpp>

#include <oni-core/network/packet.h>
#include <oni-core/entities/serialization.h>
#include <oni-core/components/geometry.h>


namespace oni {
    namespace network {
        Server::Server() = default;

        Server::~Server() = default;

        Server::Server(const Address *address, common::uint8 numClients, common::uint8 numChannels) :
                Peer::Peer(address, numClients, numChannels, 0, 0) {
        }

        void Server::postConnectHook(const ENetEvent *event) {

        }

        void Server::postDisconnectHook(const ENetEvent *event) {

        }

        void Server::handle(ENetPeer *peer, enet_uint8 *data, size_t size, PacketType header) {
            switch (header) {
                case (PacketType::PING): {
                    auto packet = deserialize<PingPacket>(data, size);
                    std::cout << packet.timestamp << std::endl;

                    send(data, size, peer);

                    break;
                }
                case (PacketType::MESSAGE): {
                    auto packet = deserialize<DataPacket>(data, size);
                    std::cout << packet.data << std::endl;
                    break;
                }
                case (PacketType::ENTITY): {

                    auto entityData = std::string(reinterpret_cast<char *>(data), size);

                    entt::DefaultRegistry reg;
                    entities::deserialization(reg, entityData);

                    auto view = reg.view<components::Shape>();
                    for (auto e: view) {
                        auto a = reg.get<components::Shape>(e).vertexA;
                        auto d = reg.get<components::Shape>(e).vertexD;
                    }

                    break;
                }
                default: {
                    // TODO: Need to keep stats on clients with bad packets and block them when threshold reaches.
                    std::cout << "Unknown packet!" << std::endl;
                    break;
                }
            }
        }

        void Server::sendWorldData(std::string &&data) {
            auto type = PacketType::WORLD_DATA;
            auto dataPacket = DataPacket{std::move(data)};
            auto packet = serialize<DataPacket>(dataPacket);

            broadcast(type, std::move(packet));
        }
    }
}
