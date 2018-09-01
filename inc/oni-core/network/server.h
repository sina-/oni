#pragma once

#include <map>

#include <enet/enet.h>

#include <oni-core/common/typedefs.h>
#include <oni-core/network/peer.h>
#include <oni-core/network/packet.h>
#include <oni-core/io/input.h>

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

            void sendEntitiesDelta(entities::EntityManager &manager);

            void sendCarEntityID(common::EntityID entityID, const common::PeerID& id);

        private:
            Server();

            void handle(ENetPeer *peer, enet_uint8 *data, size_t size, PacketType header) override;

            void postConnectHook(const ENetEvent *event) override;

            void postDisconnectHook(const ENetEvent *event) override;
        };
    }
}
