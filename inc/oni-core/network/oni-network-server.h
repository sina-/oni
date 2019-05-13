#pragma once

#include <map>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/network/oni-network-peer.h>
#include <oni-core/network/oni-network-packet.h>
#include <oni-core/component/oni-component-visual.h>

namespace oni {
    namespace physics {
        class Dynamics;
    }

    namespace entities {
        class EntityManager;

        class EntityFactory;
    }

    namespace game {
        struct Event_Collision;
        struct Event_SoundPlay;
        struct Event_RocketLaunch;
    }

    namespace network {

        class Server : public Peer {
        public:
            Server(const Address *,
                   common::u8,
                   common::u8);

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

        public:
            void
            handleEvent_Collision(const oni::game::Event_Collision &event);

            void
            handleEvent_SoundPlay(const oni::game::Event_SoundPlay &event);

            void
            handleEvent_RocketLaunch(const oni::game::Event_RocketLaunch &event);

        private:
            Server();

            void
            handle(ENetPeer *,
                   common::u8 *,
                   size_t,
                   PacketType) override;

            void
            postConnectHook(const ENetEvent *) override;

            void
            postDisconnectHook(const ENetEvent *) override;

        private:
        };
    }
}
