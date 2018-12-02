#pragma once

#include <memory>

#include <oni-core/game/game.h>
#include <oni-core/common/typedefs.h>
#include <oni-core/components/geometry.h>
#include <oni-core/network/peer.h>


namespace oni {
    namespace gameplay {
        class LapTracker;
    }

    namespace network {
        class Server;
    }

    namespace entities {
        class EntityManager;

        class ClientDataManager;

        class TileWorld;
    }

    namespace physics {
        class Dynamics;
    }

    namespace server {

        class ServerGame : public game::Game {
        public:
            explicit ServerGame(const network::Address &address);

            ~ServerGame() override;

            ServerGame(const ServerGame &) = delete;

            ServerGame &operator=(ServerGame &) = delete;

            bool shouldTerminate() override;

            void loadLevel();

        protected:
            void _sim(common::real64 simTime) override;

            void _render() override;

            void _display() override;

            void _poll() override;

            void showFPS(oni::common::int16 fps) override;

            void showSPS(oni::common::int16 tps) override;

            void showPPS(oni::common::int16 pps) override;

            void showRET(oni::common::int16 ret) override;

            void showPET(oni::common::int16 pet) override;

            void showSET(oni::common::int16 set) override;

        private:
            void setupSessionPacketHandler(const common::PeerID &, const std::string &data);

            void clientInputPacketHandler(const common::PeerID &, const std::string &data);

            void postDisconnectHook(const common::PeerID &);

            common::EntityID createCar();

            common::EntityID createTire(common::EntityID carEntityID, const math::vec3 &pos, const math::vec2 &size);

            void removeCar(common::EntityID entityID);

            void removeTire(common::EntityID carEntityID, common::EntityID tireEntityID);

            common::EntityID createTruck();

        private:
            std::unique_ptr<entities::EntityManager> mEntityManager{};

            std::unique_ptr<physics::Dynamics> mDynamics{};
            std::unique_ptr<entities::TileWorld> mTileWorld{};
            std::unique_ptr<gameplay::LapTracker> mLapTracker{};

            network::Address mServerAddress{};
            std::unique_ptr<network::Server> mServer{};

            common::EntityID mTruckEntity{};
            common::EntityID mBoxEntity{};

            std::unique_ptr<entities::ClientDataManager> mClientDataManager{};

            components::CarConfig mCarConfigDefault{};
        };
    }
}
