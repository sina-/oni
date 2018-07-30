#pragma once

#include <enet/enet.h>

#include <oni-core/network/peer.h>
#include <oni-core/entities/create-entity.h>

namespace oni {
    namespace network {
        class Client : public Peer {
        public:
            Client(entt::DefaultRegistry &foregroundEntities);

            ~Client() override;

            void connect(const Address &address);

            void pingServer();

            void sendMessage(std::string &&message);

            // TODO: Client shouldn't care about this at all.
            entities::entityID getCarEntity() const;

            // TODO: This is messed up
            bool mForegroundEntitiesReady{false};

        private:
            void handle(ENetPeer *peer, enet_uint8 *data, size_t size, PacketType header) override;

            void postConnectHook(const ENetEvent *event) override;

            void postDisconnectHook(const ENetEvent *event) override;

        private:
            ENetPeer *mEnetPeer;

            entities::entityID mCarEntity{0};

            entt::DefaultRegistry &mForegroundEntities;

        };
    }
}
