#pragma once

#include <enet/enet.h>

namespace oni {
    namespace network {
        class Client {
        public:
            Client();

            ~Client();

        private:
            ENetHost * mClient;
        };
    }
}
