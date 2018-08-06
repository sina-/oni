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
        typedef common::uint32 clientID;

        class Server : public Peer {
        public:
            Server(const Address *address, common::uint8 numClients, common::uint8 numChannels);

            ~Server() override;

            void sendForegroundEntities(entt::DefaultRegistry &registry);

            void sendBackgroundEntities(entt::DefaultRegistry &registry);

            void tick(entt::DefaultRegistry &registry);

            const std::vector<clientID> &getClients() const;

        public:
            void registerSetupSessionPacketHandler(std::function<entities::entityID(network::clientID)> &&handler);

            void registerClientInputPacketHandler(std::function<void(network::clientID, io::Input)> &&handler);

        private:
            Server();

            void handle(ENetPeer *peer, enet_uint8 *data, size_t size, PacketType header) override;

            void postConnectHook(const ENetEvent *event) override;

            void postDisconnectHook(const ENetEvent *event) override;

            void sendCarEntityID(clientID client, ENetPeer *peer);

        private:
            std::vector<clientID> mClients{};

        private:
            std::function<entities::entityID(network::clientID)> mSetupSessionPacketHandler{};
            std::function<void(network::clientID, io::Input)> mClientInputPacketHandler{};
        };
    }
}
