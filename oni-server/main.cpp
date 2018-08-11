#include <oni-core/network/server.h>
#include <oni-server/game/server-game.h>

#include <thread>

int main() {

    auto address = oni::network::Address{"127.0.0.1", 6666};
    auto server = std::make_unique<oni::server::ServerGame>(address);
    server->run();

    return 0;
}
