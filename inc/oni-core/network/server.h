#pragma once

#include <map>

#include <enet/enet.h>
#include <entt/entity/registry.hpp>

#include <oni-core/common/typedefs.h>
#include <oni-core/network/peer.h>
#include <oni-core/network/packet.h>
#include <oni-core/entities/create-entity.h>

namespace oni {
    namespace physics {
        class Dynamics;
    }
    namespace network {
        class Server : public Peer {
        public:
            Server(const Address *address, common::uint8 numClients, common::uint8 numChannels,
                   entt::DefaultRegistry &foregroundEntities, physics::Dynamics &dynamics);

            void sendWorldData(entt::DefaultRegistry &registry);

            void tick(entt::DefaultRegistry &registry);

            std::vector<entities::entityID> getCarEntities() const;

            ~Server() override;

        private:
            Server();

            void handle(ENetPeer *peer, enet_uint8 *data, size_t size, PacketType header) override;

            void postConnectHook(const ENetEvent *event) override;

            void postDisconnectHook(const ENetEvent *event) override;

        private:
            entt::DefaultRegistry &mForegroundEntities;
            physics::Dynamics &mDynamics;

            std::map<common::uint32, entities::entityID> mClientToCarEntity{};
        };
    }
}
