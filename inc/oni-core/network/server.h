#pragma once

#include <map>

#include <oni-core/common/typedefs.h>
#include <oni-core/network/peer.h>
#include <oni-core/network/packet.h>
#include <oni-core/component/visual.h>

namespace oni {
    namespace physics {
        class Dynamics;
    }
    namespace entities {
        class EntityManager;

        class EntityFactory;
    }
    namespace network {

        class Server : public Peer {
        public:
            Server(const Address *,
                   common::uint8,
                   common::uint8);

            ~Server() override;

            void
            sendEntitiesAll(entities::EntityManager &);

            void
            sendComponentsUpdate(entities::EntityManager &);

            void
            sendNewEntities(entities::EntityManager &);

            void
            broadcastDeletedEntities(entities::EntityManager &);

            void
            sendCarEntityID(common::EntityID,
                            const common::PeerID &);

            void
            broadcastEvents(entities::EntityFactory &entityFactory);

        private:
            Server();

            void
            handle(ENetPeer *,
                   common::uint8 *,
                   size_t,
                   PacketType) override;

            void
            postConnectHook(const ENetEvent *) override;

            void
            postDisconnectHook(const ENetEvent *) override;
        };
    }
}
