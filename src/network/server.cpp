#include <utility>

#include <oni-core/network/server.h>

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <cassert>

#include <Box2D/Box2D.h>
#include <enet/enet.h>
#include <entt/entity/registry.hpp>
#include <cereal/archives/portable_binary.hpp>

#include <oni-core/network/packet.h>
#include <oni-core/entities/serialization.h>
#include <oni-core/components/geometry.h>
#include <oni-core/entities/manage-hierarchy.h>
#include <oni-core/physics/dynamics.h>

// TODO: This shouldn't be included here!
#include <oni-core/graphics/debug-draw-box2d.h>


namespace oni {
    namespace network {
        Server::Server(const Address *address, common::uint8 numClients, common::uint8 numChannels) :
                Peer::Peer(address, numClients, numChannels, 0, 0) {
        }

        Server::~Server() = default;

        void Server::postConnectHook(const ENetEvent *event) {
            mClients.push_back(event->peer->connectID);
        }

        void Server::postDisconnectHook(const ENetEvent *event) {
            auto clientID = event->peer->connectID;
            auto iter = mClients.begin();
            while (iter != mClients.end()) {
                if (*iter == clientID) {
                    mClients.erase(iter);
                }
                ++iter;
            }
        }

        void Server::tick(entt::DefaultRegistry &registry) {

        }

        void Server::handle(ENetPeer *peer, enet_uint8 *data, size_t size, PacketType header) {
            switch (header) {
                case (PacketType::PING): {
                    auto packet = deserialize<PingPacket>(data, size);
                    std::cout << packet.timestamp << std::endl;

                    // TODO: Can't just send the packet like this, its missing the header and the size is only
                    // the size of payload
                    //send(data, size, peer);

                    break;
                }
                case (PacketType::MESSAGE): {
                    auto packet = deserialize<DataPacket>(data, size);
                    std::cout << packet.data << std::endl;

                    break;
                }
                case (PacketType::SETUP_SESSION): {
                    auto carEntity = mSetupSessionPacketHandler(peer->connectID);
                    sendCarEntityID(carEntity, peer);
                    break;
                }
                case (PacketType::CLIENT_INPUT): {
                    auto input = deserialize<io::Input>(data, size);
                    mClientInputPacketHandler(peer->connectID, input);

                    break;
                }
                default: {
                    // TODO: Need to keep stats on clients with bad packets and block them when threshold reaches.
                    std::cout << "Unknown packet!" << std::endl;

                    break;
                }
            }
        }

        void Server::sendForegroundEntities(entt::DefaultRegistry &registry) {
            std::string data = entities::serialize(registry);
            auto type = PacketType::FOREGROUND_ENTITIES;

            broadcast(type, data);
        }

        void Server::sendBackgroundEntities(entt::DefaultRegistry &registry) {
            std::string data = entities::serialize(registry);
            auto type = PacketType::BACKGROUND_ENTITIES;

            broadcast(type, data);
        }

        const std::vector<clientID> &Server::getClients() const {
            return mClients;
        }

        void Server::sendCarEntityID(entities::entityID entityID, ENetPeer *peer) {
            auto packet = EntityPacket{entityID};
            auto data = serialize<EntityPacket>(packet);
            auto type = PacketType::CAR_ENTITY_ID;

            send(type, data, peer);
        }

        void Server::registerSetupSessionPacketHandler(std::function<entities::entityID(network::clientID)> &&handler) {
            mSetupSessionPacketHandler = std::move(handler);
        }

        void Server::registerClientInputPacketHandler(std::function<void(network::clientID, io::Input)> &&handler) {
            mClientInputPacketHandler = std::move(handler);
        }
    }
}
