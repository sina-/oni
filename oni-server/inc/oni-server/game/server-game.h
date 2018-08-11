#pragma once

#include <memory>

#include <AntTweakBar.h>
#include <entt/entt.hpp>
#include <Box2D/Box2D.h>

#include <oni-core/game/game.h>
#include <oni-core/physics/dynamics.h>
#include <oni-core/common/typedefs.h>
#include <oni-core/entities/create-entity.h>
#include <oni-core/components/geometry.h>
#include <oni-core/entities/tile-world.h>
#include <oni-core/graphics/debug-draw-box2d.h>
#include <oni-core/network/client.h>
#include <oni-core/network/server.h>


namespace oni {
    namespace server {
        typedef std::map<network::PeerID, io::Input> ClientInputMap;
        typedef std::map<network::PeerID, entities::EntityID> ClientCarEntityMap;

        class ServerGame : public game::Game {
        public:
            explicit ServerGame(const network::Address &address);

            ~ServerGame() override;

            ServerGame(const ServerGame &) = delete;

            ServerGame &operator=(ServerGame &) = delete;

            bool shouldTerminate() override;

            void loadLevel();

        protected:
            void _tick(common::real32 tickTime) override;

            void _render() override;

            void _display() override;

            void _poll() override;

            void showFPS(int16 fps) override;

            void showTPS(int16 tps) override;

            void showFET(common::int16 fet) override;

        private:
            void setupSessionPacketHandler(network::PeerID, const std::string &data);

            void clientInputPacketHandler(network::PeerID, const std::string &data);

        private:
            // TODO: Think about wrapping these in a struct and passing that to the systems and letting the system
            // decide which group of entities it needs to interact with.
            std::unique_ptr<entt::DefaultRegistry> mForegroundEntities{};
            std::unique_ptr<entt::DefaultRegistry> mBackgroundEntities{};

            std::unique_ptr<physics::Dynamics> mDynamics{};
            std::unique_ptr<entities::TileWorld> mTileWorld{};

            network::Address mServerAddress{};
            std::unique_ptr<network::Server> mServer{};

            entities::EntityID mTruckEntity{};
            entities::EntityID mBoxEntity{};

            ClientInputMap mClientInputMap{};
            ClientCarEntityMap mClientCarEntityMap{};

            components::CarConfig mCarConfigDefault{};
        };
    }
}
