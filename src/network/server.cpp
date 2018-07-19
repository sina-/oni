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

        void Server::handle(ENetEvent *event) {
            // TODO: Need to gather stats on invalid packets and there source!
            if (!event->packet->data) {
                return;
            }
            if (!event->packet->dataLength) {
                return;
            }

            auto data = event->packet->data;
            auto header = getHeader(data);
            auto headerSize = 1;
            auto dataWithoutHeaderSize = event->packet->dataLength - headerSize;
            data += 1;

            switch (header) {
                case (PacketType::PING): {
                    auto packet = deserialize<PingPacket>(data);
                    std::cout << packet.timestamp << std::endl;

                    send(data, event->packet->dataLength, event->peer);

                    break;
                }
                case (PacketType::MESSAGE): {
                    auto packet = deserialize<MessagePacket>(data);
                    std::cout << packet.message << std::endl;
                    break;
                }
                case (PacketType::ENTITY): {

                    auto entityData = std::string(reinterpret_cast<char *>(data), event->packet->dataLength);

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

        void Server::sendWorldData(std::string &&data) const {

        }
    }
}
