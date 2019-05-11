#pragma once

#include <memory>

#include <oni-core/game/game.h>
#include <oni-core/common/typedefs.h>
#include <oni-core/component/geometry.h>
#include <oni-core/network/peer.h>
#include <oni-server/entities/tile-world.h>
#include <oni-core/component/visual.h>


namespace oni {
    namespace gameplay {
        class LapTracker;
    }

    namespace network {
        class Server;
    }

    namespace math {
        class ZLayerManager;
    }

    namespace entities {
        class EntityManager;

        class EntityFactory;

        class ClientDataManager;
    }

    namespace physics {
        class Dynamics;
    }

    namespace server {
        namespace game {
            using EntityID = oni::common::EntityID;

            class ServerGame : public oni::game::Game {
            public:
                explicit ServerGame(const oni::network::Address &address);

                ~ServerGame() override;

                ServerGame(const ServerGame &) = delete;

                ServerGame &
                operator=(ServerGame &) = delete;

                bool
                shouldTerminate() override;

                void
                loadLevel();

            protected:
                void
                _sim(oni::common::real64 simTime) override;

                void
                _render(oni::common::real64 simTime) override;

                void
                _display() override;

                void
                _poll() override;

                void
                _finish() override;

                void
                showFPS(oni::common::int16 fps) override;

                void
                showSPS(oni::common::int16 tps) override;

                void
                showPPS(oni::common::int16 pps) override;

                void
                showRT(oni::common::int16) override;

                void
                showPT(oni::common::int16) override;

                void
                showST(oni::common::int16) override;

            private:
                void
                setupSessionPacketHandler(const oni::common::PeerID &,
                                          const std::string &);

                void
                clientInputPacketHandler(const oni::common::PeerID &,
                                         const std::string &);

                void
                postDisconnectHook(const oni::common::PeerID &);

                EntityID
                spawnRaceCar();

                EntityID
                spawnTruck();

            private:
                std::unique_ptr<oni::entities::EntityFactory> mEntityFactory{};

                std::unique_ptr<oni::math::ZLayerManager> mZLayerManager{};
                std::unique_ptr<oni::physics::Dynamics> mDynamics{};
                std::unique_ptr<server::entities::TileWorld> mTileWorld{};
                std::unique_ptr<oni::gameplay::LapTracker> mLapTracker{};

                network::Address mServerAddress{};
                std::unique_ptr<oni::network::Server> mServer{};

                EntityID mTruckEntity{};
                EntityID mBoxEntity{};

                std::unique_ptr<oni::entities::ClientDataManager> mClientDataManager{};

                oni::component::CarConfig mCarConfigDefault{};
            };
        }
    }
}
