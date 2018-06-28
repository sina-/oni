#include <oni-core/network/client.h>

#include <stdexcept>

namespace oni {
    namespace network {

        Client::Client() {
            mClient = enet_host_create(nullptr, 1, 2, 0, 0);
            if (mClient == nullptr) {
                std::runtime_error("An error occurred while creating client.\n");
            }
        }

        Client::~Client() {
            enet_host_destroy(mClient);
        }
    }
}