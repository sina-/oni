#include <oni-core/network/server.h>

#include <enet/enet.h>

#include <oni-core/entities/entity-manager.h>
#include <oni-core/entities/serialization.h>


namespace oni {
    namespace network {
        Server::Server(const Address *address, common::uint8 numClients, common::uint8 numChannels) :
                Peer::Peer(address, numClients, numChannels, 0, 0) {
        }

        Server::Server() = default;

        Server::~Server() = default;

        void Server::postConnectHook(const ENetEvent *event) {
        }

        void Server::postDisconnectHook(const ENetEvent *event) {
            auto clientID = getPeerID(*event->peer);

            mPostDisconnectHook(clientID);
        }

        void Server::handle(ENetPeer *peer, enet_uint8 *data, size_t size, PacketType header) {
            auto peerID = getPeerID(*peer);
            switch (header) {
                case (PacketType::PING): {
                    auto type = PacketType::PING;
                    auto pingData = std::string{};

                    send(type, pingData, peer);

                    break;
                }
                case (PacketType::MESSAGE): {
                    auto packet = entities::deserialize<DataPacket>(data, size);
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
                case (PacketType::Z_LAYER): {
                    auto dataString = std::string(reinterpret_cast<char *>(data), size);
                    mPacketHandlers[PacketType::Z_LAYER](peerID, dataString);
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
            std::string data = entities::serialize(manager, component::SnapshotType::ENTIRE_REGISTRY);
            auto type = PacketType::REPLACE_ALL_ENTITIES;

            if (data.size() > 1) {
                broadcast(type, data);
            }
        }

        void Server::sendComponentsUpdate(entities::EntityManager &manager) {
            std::string data = entities::serialize(manager, component::SnapshotType::ONLY_COMPONENTS);
            auto type = PacketType::ONLY_COMPONENT_UPDATE;

            if (data.size() > 1) {
                broadcast(type, data);

                auto lock = manager.scopedLock();
                // TODO: What happens if broadcast fails for some clients? Would they miss these entities forever?
                manager.reset<component::Tag_OnlyComponentUpdate>();
            }
        }

        void Server::sendNewEntities(entities::EntityManager &manager) {
            std::string data = entities::serialize(manager, component::SnapshotType::ONLY_NEW_ENTITIES);
            auto type = PacketType::ADD_NEW_ENTITIES;

            if (data.size() > 1) {
                broadcast(type, data);

                auto lock = manager.scopedLock();
                // TODO: What happens if broadcast fails for some clients? Would they miss these entities forever?
                manager.reset<component::Tag_NewEntity>();
            }
        }

        void Server::broadcastDeletedEntities(entities::EntityManager &manager) {
            auto type = PacketType::DESTROYED_ENTITIES;
            auto data = entities::serialize<std::vector<common::EntityID>>(manager.getDeletedEntities());

            if (data.size() > 1) {
                broadcast(type, data);
            }
        }

        void Server::sendCarEntityID(common::EntityID entityID, const common::PeerID &peerID) {
            auto packet = EntityPacket{entityID};
            auto data = entities::serialize<network::EntityPacket>(packet);
            auto type = PacketType::CAR_ENTITY_ID;

            send(type, data, mPeers[peerID]);
        }

        void Server::sendZLayer(const common::PeerID &peerID, const component::ZLayer &zLayer) {
            ZLayerPacket zLayerPacket{};
            zLayerPacket.level_0 = zLayer.at(component::ZLayerDef::LAYER_0);
            zLayerPacket.level_1 = zLayer.at(component::ZLayerDef::LAYER_1);
            zLayerPacket.level_2 = zLayer.at(component::ZLayerDef::LAYER_2);
            zLayerPacket.level_3 = zLayer.at(component::ZLayerDef::LAYER_3);
            zLayerPacket.level_4 = zLayer.at(component::ZLayerDef::LAYER_4);
            zLayerPacket.level_5 = zLayer.at(component::ZLayerDef::LAYER_5);
            zLayerPacket.level_6 = zLayer.at(component::ZLayerDef::LAYER_6);
            zLayerPacket.level_7 = zLayer.at(component::ZLayerDef::LAYER_7);
            zLayerPacket.level_8 = zLayer.at(component::ZLayerDef::LAYER_8);
            zLayerPacket.level_9 = zLayer.at(component::ZLayerDef::LAYER_9);

            auto data = entities::serialize<ZLayerPacket>(zLayerPacket);
            auto type = PacketType::Z_LAYER;

            send(type, data, mPeers[peerID]);
        }

        void Server::broadcastSpawnParticle(entities::EntityManager &manager) {
            std::vector<component::Particle> particles;
            {
                auto view = manager.createViewScopeLock<component::Particle>();
                for (auto &&entity: view) {
                    particles.emplace_back(view.get<component::Particle>(entity));
                }
                manager.reset<component::Particle>();
            }

            // TODO: This is just way to many packets. I should batch them together.
            for (auto &&particle: particles) {
                auto data = entities::serialize<component::Particle>(particle);
                auto type = PacketType::SPAWN_PARTICLE;

                broadcast(type, data);
            }
        }
    }
}
