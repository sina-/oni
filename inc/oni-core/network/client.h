#pragma once

#include <enet/enet.h>

#include <oni-core/network/peer.h>
#include <oni-core/entities/create-entity.h>

namespace oni {

    namespace io {
        class Input;
    }

    namespace network {
        class Client : public Peer {
        public:
            Client();

            ~Client() override;

            void connect(const Address &address);

            void pingServer();

            void sendMessage(std::string &&message);

            void sendInput(const io::Input *input);

        public:
            void registerCarEntityIDPacketHandler(std::function<void(entities::entityID)> &&handler);

            void registerForegroundEntitiesPacketHandler(std::function<void(const std::string &)> &&handler);

            void registerBackgroundEntitiesPacketHandler(std::function<void(const std::string &)> &&handler);

        private:
            void handle(ENetPeer *peer, enet_uint8 *data, size_t size, PacketType header) override;

            void postConnectHook(const ENetEvent *event) override;

            void postDisconnectHook(const ENetEvent *event) override;

            void requestSessionSetup();

        private:
            ENetPeer *mEnetPeer;

        private:
            std::function<void(entities::entityID)> mCarEntityIDPacketHandler{};
            std::function<void(const std::string &)> mForegroundEntitiesPacketHandler{};
            std::function<void(const std::string &)> mBackgroundEntitiesPacketHandler{};
        };
    }
}
