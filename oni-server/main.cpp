#include <oni-core/network/server.h>

#include <thread>

int main() {

    auto address = oni::network::Address{"127.0.0.1", 6666};
    auto server = std::make_unique<oni::network::Server>(&address, 16, 2);
    while (true) {
        server->poll();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}
