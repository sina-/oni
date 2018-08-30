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


namespace oni {
    namespace network {
        Server::Server(const Address *address, common::uint8 numClients, common::uint8 numChannels) :
                Peer::Peer(address, numClients, numChannels, 0, 0) {
        }

        Server::~Server() = default;

        void Server::postConnectHook(const ENetEvent *event) {
            mClients.push_back(getPeerID(event->peer->address));
        }

        void Server::postDisconnectHook(const ENetEvent *event) {
            auto clientID = getPeerID(event->peer->address);

            mPostDisconnectHook(clientID);

            auto it = std::find_if(mClients.begin(), mClients.end(), [&](const common::PeerID& peerID) { return (peerID == clientID); });
            if (it != mClients.end()) {
                mClients.erase(it);
            }
            else {
                assert(false);
            }
        }

        void Server::tick(entities::EntityManager &manager) {

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

        void Server::sendEntities(entities::EntityManager &manager) {
            std::string data = entities::serialize(manager);
            auto type = PacketType::ENTITIES;

            broadcast(type, data);
        }

        const std::vector<common::PeerID> &Server::getClients() const {
            return mClients;
        }

        void Server::sendCarEntityID(common::EntityID entityID, const common::PeerID& peerID) {
            auto packet = EntityPacket{entityID};
            auto data = serialize<EntityPacket>(packet);
            auto type = PacketType::CAR_ENTITY_ID;

            send(type, data, mPeers[peerID]);
        }
    }
}
