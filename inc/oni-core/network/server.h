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
        typedef std::map<common::uint32, entities::entityID> clientCarEntityMap;
        typedef std::map<common::uint32, io::Input> clientInputMap;
        typedef common::uint32 clientID;

        class Server : public Peer {
        public:
            Server(const Address *address, common::uint8 numClients, common::uint8 numChannels,
                   entt::DefaultRegistry &foregroundEntities, physics::Dynamics &dynamics);

            ~Server() override;

            void sendWorldData(entt::DefaultRegistry &registry);

            void tick(entt::DefaultRegistry &registry);

            std::vector<entities::entityID> getCarEntities() const;

            const std::vector<clientID> &getClients() const;

            entities::entityID getCarEntity(clientID id) const;

            const io::Input &getClientInput(clientID id) const;


        private:
            Server();

            void handle(ENetPeer *peer, enet_uint8 *data, size_t size, PacketType header) override;

            void postConnectHook(const ENetEvent *event) override;

            void postDisconnectHook(const ENetEvent *event) override;

        private:
            entt::DefaultRegistry &mForegroundEntities;
            physics::Dynamics &mDynamics;

            clientCarEntityMap mClientCarEntity{};
            clientInputMap mClientInput{};
            std::vector<clientID> mClients{};
        };
    }
}
