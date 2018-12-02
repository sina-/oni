#pragma once

#include <map>

#include <oni-core/common/typedefs.h>
#include <oni-core/network/peer.h>
#include <oni-core/network/packet.h>

namespace oni {
    namespace physics {
        class Dynamics;
    }
    namespace entities {
        class EntityManager;
    }
    namespace network {

        class Server : public Peer {
        public:
            Server(const Address *address, common::uint8 numClients, common::uint8 numChannels);

            ~Server() override;

            void sendEntitiesAll(entities::EntityManager &manager);

            void sendComponentsUpdate(entities::EntityManager &manager);
            
            void sendNewEntities(entities::EntityManager &manager);

            void broadCastDeletedEntities(entities::EntityManager &manager);

            void sendCarEntityID(common::EntityID entityID, const common::PeerID& id);

        private:
            Server();

            void handle(ENetPeer *peer, common::uint8 *data, size_t size, PacketType header) override;

            void postConnectHook(const ENetEvent *event) override;

            void postDisconnectHook(const ENetEvent *event) override;
        };
    }
}
