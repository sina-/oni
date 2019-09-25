#pragma once

#include <memory>

#include <oni-core/io/oni-io-fwd.h>
#include <oni-core/network/oni-network-peer.h>

namespace oni {
    namespace utils {
        class Timer;
    }

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
        sendInput(const Input *input);

        void
        requestZLevelDelta();

    private:
        void
        handle(ENetPeer *peer,
               u8 *data,
               size size,
               PacketType header) override;

        void
        postConnectHook(const ENetEvent *event) override;

        void
        postDisconnectHook(const ENetEvent *event) override;

        void
        requestSessionSetup();

    private:
        ENetPeer *mEnetServer;
        std::unique_ptr<Timer> mTimer{};
    };
}
