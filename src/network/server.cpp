#include <oni-core/network/server.h>

#include <stdexcept>

#include <enet/enet.h>

namespace oni {
    namespace network {
        Server::Server() {
            auto result = enet_initialize();
            if (result != 0) {
                std::runtime_error("An error occurred while initializing ENet.\n");
            }
        }

        Server::~Server() {
            enet_deinitialize();
        }
    }
}
