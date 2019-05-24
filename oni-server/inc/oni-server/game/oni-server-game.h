#pragma once

#include <memory>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-visual.h>
#include <oni-core/component/oni-component-physics.h>
#include <oni-core/game/oni-game.h>
#include <oni-core/network/oni-network-peer.h>

#include <oni-server/level/oni-server-level-tile-world.h>


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
                _sim(oni::common::r64 simTime) override;

                void
                _render(oni::common::r64 simTime) override;

                void
                _display() override;

                void
                _poll() override;

                void
                _finish() override;

                void
                showFPS(oni::common::i16 fps) override;

                void
                showSPS(oni::common::i16 tps) override;

                void
                showPPS(oni::common::i16 pps) override;

                void
                showRT(oni::common::i16) override;

                void
                showPT(oni::common::i16) override;

                void
                showST(oni::common::i16) override;

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
                std::unique_ptr<oni::entities::EntityManager> mEntityManager{};

                std::unique_ptr<oni::math::ZLayerManager> mZLayerManager{};
                std::unique_ptr<oni::physics::Dynamics> mDynamics{};
                std::unique_ptr<oni::server::level::TileWorld> mTileWorld{};
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
