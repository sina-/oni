#pragma once

#include <map>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/component/oni-component-visual.h>
#include <oni-core/entities/oni-entities-fwd.h>
#include <oni-core/game/oni-game-fwd.h>
#include <oni-core/network/oni-network-address.h>
#include <oni-core/network/oni-network-packet.h>
#include <oni-core/network/oni-network-peer.h>
#include <oni-core/physics/oni-physics-fwd.h>


namespace oni {
    class Server : public Peer {
    public:
        Server(const Address *,
               u8,
               u8);

        ~Server() override;

        void
        sendEntitiesAll(EntityManager &);

        void
        sendComponentsUpdate(EntityManager &);

        void
        sendNewEntities(EntityManager &);

        void
        broadcastDeletedEntities(EntityManager &);

        void
        sendCarEntityID(EntityID,
                        const std::string &);

    public:
        void
        handleEvent_Collision(const Event_Collision &event);

        void
        handleEvent_SoundPlay(const Event_SoundPlay &event);

        void
        handleEvent_RocketLaunch(const Event_RocketLaunch &event);

    private:
        Server();

        void
        handle(ENetPeer *,
               u8 *,
               size_t,
               PacketType) override;

        void
        postConnectHook(const ENetEvent *) override;

        void
        postDisconnectHook(const ENetEvent *) override;

    private:
    };
}
