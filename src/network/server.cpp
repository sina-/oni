#include <oni-core/network/server.h>

#include <enet/enet.h>

#include <oni-core/entities/entity-manager.h>
#include <oni-core/entities/entity-factory.h>
#include <oni-core/entities/serialization.h>
#include <oni-core/component/audio.h>


namespace oni {
    namespace network {
        Server::Server(const Address *address,
                       common::uint8 numClients,
                       common::uint8 numChannels) :
                Peer::Peer(address, numClients, numChannels, 0, 0) {
        }

        Server::Server() = default;

        Server::~Server() = default;

        void
        Server::postConnectHook(const ENetEvent *event) {
        }

        void
        Server::postDisconnectHook(const ENetEvent *event) {
            auto clientID = getPeerID(*event->peer);

            mPostDisconnectHook(clientID);
        }

        void
        Server::handle(ENetPeer *peer,
                       enet_uint8 *data,
                       size_t size,
                       PacketType header) {
            auto peerID = getPeerID(*peer);
            assert(mPacketHandlers.find(header) != mPacketHandlers.end() || header == PacketType::PING ||
                   header == PacketType::MESSAGE);
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
                default: {
                    // TODO: Need to keep stats on clients with bad packets and block them when threshold reaches.
                    assert(false);
                    break;
                }
            }
        }

        void
        Server::sendEntitiesAll(entities::EntityManager &manager) {
            std::string data = entities::serialize(manager, component::SnapshotType::ENTIRE_REGISTRY);
            auto type = PacketType::REGISTRY_REPLACE_ALL_ENTITIES;

            if (data.size() > 1) {
                broadcast(type, data);
            }
        }

        void
        Server::sendComponentsUpdate(entities::EntityManager &manager) {
            // TODO: What happens if broadcast fails for some clients? Would they miss these entities forever?
            std::string data = entities::serialize(manager, component::SnapshotType::ONLY_COMPONENTS);
            auto type = PacketType::REGISTRY_ONLY_COMPONENT_UPDATE;

            if (data.size() > 1) {
                broadcast(type, data);
            }
        }

        void
        Server::sendNewEntities(entities::EntityManager &manager) {
            // TODO: What happens if broadcast fails for some clients? Would they miss these entities forever?
            std::string data = entities::serialize(manager, component::SnapshotType::ONLY_NEW_ENTITIES);
            auto type = PacketType::REGISTRY_ADD_NEW_ENTITIES;

            if (data.size() > 1) {
                broadcast(type, data);
            }
        }

        void
        Server::broadcastDeletedEntities(entities::EntityManager &manager) {
            auto type = PacketType::REGISTRY_DESTROYED_ENTITIES;
            auto data = entities::serialize(manager.getDeletedEntities());
            manager.clearDeletedEntitiesList();

            if (data.size() > 1) {
                broadcast(type, data);
            }
        }

        void
        Server::sendCarEntityID(common::EntityID entityID,
                                const common::PeerID &peerID) {
            auto packet = EntityPacket{entityID};
            auto data = entities::serialize(packet);
            auto type = PacketType::CAR_ENTITY_ID;

            send(type, data, mPeers[peerID]);
        }

        void
        Server::broadcastEvents(entities::EntityFactory &entityFactory) {
            std::vector<CollisionEventPacket> collisionPackets;
            {
                std::function<void(component::CollidingEntity &,
                                   component::CollisionPos &)> func = [&collisionPackets](
                        component::CollidingEntity &collidingEntity,
                        component::CollisionPos &collisionPos) {
                    CollisionEventPacket packet;
                    packet.collidingEntity = collidingEntity;
                    packet.collisionPos = collisionPos;
                    collisionPackets.emplace_back(packet);
                };
                entityFactory.apply<component::EventType::COLLISION>(func);
            }

            std::vector<SoundPlayEventPacket> soundPlayPackets;
            {
                std::function<void(component::SoundID &,
                                   component::SoundPos &)> func = [&soundPlayPackets](
                        component::SoundID &soundEffectID,
                        component::SoundPos &soundPos) {
                    SoundPlayEventPacket packet;
                    packet.soundID = soundEffectID;
                    packet.pos = soundPos;
                    soundPlayPackets.emplace_back(packet);
                };
                entityFactory.apply<component::EventType::ONE_SHOT_SOUND_EFFECT>(func);
            }

            std::vector<RocketLaunchEventPacket> rocketLaunchPackets;
            {
                std::function<void(math::vec2 &pos)> func = [&rocketLaunchPackets](math::vec2 &pos) {
                    RocketLaunchEventPacket packet;
                    packet.pos = pos;
                    rocketLaunchPackets.emplace_back(packet);
                };
                entityFactory.apply<component::EventType::ROCKET_LAUNCH>(func);
            }

            entityFactory.resetEvents();

            auto data = entities::serialize(collisionPackets);
            auto type = PacketType::EVENT_COLLISION;

            broadcast(type, data);

            data = entities::serialize(soundPlayPackets);
            type = PacketType::EVENT_ONE_SHOT_SOUND_EFFECT;

            broadcast(type, data);

            data = entities::serialize(rocketLaunchPackets);
            type = PacketType::EVENT_ROCKET_LAUNCH;

            broadcast(type, data);
        }
    }
}
