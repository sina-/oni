#pragma once

#include <oni-core/network/peer.h>

namespace oni {

    namespace utils {
        class Timer;
    }
    namespace io {
        class Input;
    }

    namespace network {
        class Client : public Peer {
        public:
            Client();

            ~Client() override;

            void
            connect(const Address &address);

            void
            disconnect();

            void
            pingServer();

            void
            sendMessage(std::string &&message);

            void
            sendInput(const io::Input *input);

            void
            requestZLevelDelta();

        private:
            void
            handle(ENetPeer *peer,
                   common::uint8 *data,
                   size_t size,
                   PacketType header) override;

            void
            postConnectHook(const ENetEvent *event) override;

            void
            postDisconnectHook(const ENetEvent *event) override;

            void
            requestSessionSetup();

        private:
            ENetPeer *mEnetServer;
            std::unique_ptr<utils::Timer> mTimer{};
        };
    }
}
