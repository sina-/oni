#pragma once

#include <enet/enet.h>

#include <oni-core/network/peer.h>

namespace oni {

    namespace utils {
        class HighResolutionTimer;
    }
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

        private:
            void handle(ENetPeer *peer, enet_uint8 *data, size_t size, PacketType header) override;

            void postConnectHook(const ENetEvent *event) override;

            void postDisconnectHook(const ENetEvent *event) override;

            void requestSessionSetup();

        private:
            ENetPeer *mEnetPeer;
            std::unique_ptr<utils::HighResolutionTimer> mTimer{};
        };
    }
}
