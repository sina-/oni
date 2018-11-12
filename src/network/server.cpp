#include <utility>

#include <oni-core/network/server.h>

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <cassert>

#include <Box2D/Box2D.h>
#include <cereal/archives/portable_binary.hpp>

#include <oni-core/network/packet.h>
#include <oni-core/entities/serialization.h>
#include <oni-core/entities/entity-manager.h>
#include <oni-core/components/geometry.h>
#include <oni-core/entities/manage-hierarchy.h>
#include <oni-core/physics/dynamics.h>

// TODO: This shouldn't be included here!
#include <oni-core/graphics/debug-draw-box2d.h>
#include <oni-core/components/world-data-status.h>


namespace oni {
    namespace network {
        Server::Server(const Address *address, common::uint8 numClients, common::uint8 numChannels) :
                Peer::Peer(address, numClients, numChannels, 0, 0) {
        }

        Server::~Server() = default;

        void Server::postConnectHook(const ENetEvent *event) {
        }

        void Server::postDisconnectHook(const ENetEvent *event) {
            auto clientID = getPeerID(event->peer->address);

            mPostDisconnectHook(clientID);
        }

        void Server::handle(ENetPeer *peer, enet_uint8 *data, size_t size, PacketType header) {
            auto peerID = getPeerID(peer->address);
            switch (header) {
                case (PacketType::PING): {
                    auto type = PacketType::PING;
                    auto pingData = std::string{};

                    send(type, pingData, peer);

                    break;
                }
                case (PacketType::MESSAGE): {
                    auto packet = deserialize<DataPacket>(data, size);
                    break;
                }
                case (PacketType::SETUP_SESSION): {
                    mPacketHandlers[PacketType::SETUP_SESSION](peerID, "");
                    break;
                }
                case (PacketType::CLIENT_INPUT): {
                    auto dataString = std::string(reinterpret_cast<char *>(data), size);
                    mPacketHandlers[PacketType::CLIENT_INPUT](peerID, dataString);
                    break;
                }
                default: {
                    // TODO: Need to keep stats on clients with bad packets and block them when threshold reaches.
                    std::cout << "Unknown packet!" << std::endl;

                    break;
                }
            }
        }

        void Server::sendEntitiesAll(entities::EntityManager &manager) {
            std::string data = entities::serialize(manager, components::WorldDataStatus::REPLACE_ALL_ENTITIES);
            auto type = PacketType::REPLACE_ALL_ENTITIES;

            if (data.size() > 1) {
                broadcast(type, data);
            }
        }

        void Server::sendComponentsUpdate(entities::EntityManager &manager) {
            std::string data = entities::serialize(manager, components::WorldDataStatus::ONLY_COMPONENT_UPDATE);
            auto type = PacketType::ONLY_COMPONENT_UPDATE;

            if (data.size() > 1) {
                broadcast(type, data);

                auto lock = manager.scopedLock();
                // TODO: What happens if broadcast fails for some clients? Would they miss these entities forever?
                manager.reset<components::TagOnlyComponentUpdate>();
            }
        }

        void Server::sendNewEntities(entities::EntityManager &manager) {
            std::string data = entities::serialize(manager, components::WorldDataStatus::ADD_NEW_ENTITIES);
            auto type = PacketType::ADD_NEW_ENTITIES;

            if (data.size() > 1) {
                broadcast(type, data);

                auto lock = manager.scopedLock();
                // TODO: What happens if broadcast fails for some clients? Would they miss these entities forever?
                manager.reset<components::TagAddNewEntities>();
            }
        }

        void Server::sendRemainingEntitiesAfterDelete(entities::EntityManager &manager) {
            //std::string data = entities::serialize(manager, components::WorldDataStatus::REMOVE_NON_EXISTING_ENTITIES);
            auto type = PacketType::DESTROYED_ENTITIES;
            auto data = serialize<std::vector<common::EntityID>>(manager.getDeletedEntities());

            if (data.size() > 1) {
                broadcast(type, data);
            }
        }

        void Server::sendCarEntityID(common::EntityID entityID, const common::PeerID &peerID) {
            auto packet = EntityPacket{entityID};
            auto data = serialize<EntityPacket>(packet);
            auto type = PacketType::CAR_ENTITY_ID;

            send(type, data, mPeers[peerID]);
        }
    }
}
