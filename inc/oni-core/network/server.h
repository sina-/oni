#pragma once

#include <map>

#include <enet/enet.h>
#include <entt/entity/registry.hpp>

#include <oni-core/common/typedefs.h>
#include <oni-core/network/peer.h>
#include <oni-core/network/packet.h>
#include <oni-core/entities/create-entity.h>
#include <oni-core/io/input.h>

namespace oni {
    namespace physics {
        class Dynamics;
    }
    namespace network {

        class Server : public Peer {
        public:
            Server(const Address *address, common::uint8 numClients, common::uint8 numChannels);

            ~Server() override;

            void sendForegroundEntities(entt::DefaultRegistry &registry);

            void sendBackgroundEntities(entt::DefaultRegistry &registry);

            void tick(entt::DefaultRegistry &registry);

            const std::vector<PeerID> &getClients() const;

            void sendCarEntityID(entities::EntityID entityID, PeerID id);

        private:
            Server();

            void handle(ENetPeer *peer, enet_uint8 *data, size_t size, PacketType header) override;

            void postConnectHook(const ENetEvent *event) override;

            void postDisconnectHook(const ENetEvent *event) override;

        private:
            std::vector<PeerID> mClients{};
        };
    }
}
