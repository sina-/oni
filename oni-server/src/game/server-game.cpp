#include <oni-server/game/server-game.h>

#include <thread>

#include <entt/entt.hpp>

#include <oni-core/entities/create-entity.h>
#include <oni-core/io/output.h>
#include <oni-core/audio/audio-manager-fmod.h>
#include <oni-core/physics/transformation.h>
#include <oni-core/common/consts.h>
#include <oni-core/entities/manage-hierarchy.h>
#include <oni-core/entities/serialization.h>
#include <oni-core/network/server.h>
#include <oni-core/io/input.h>


namespace oni {
    namespace server {
        ServerGame::ServerGame(const network::Address &address) : Game(), mServerAddress(address) {
            srand(static_cast<unsigned int>(time(nullptr)));

            // TODO: probably better to wrap this in a class rather than using it naked, but doing so requires lot of
            // coding and time that is better spent working on other features
            mForegroundEntities = std::make_unique<entt::DefaultRegistry>();
            mBackgroundEntities = std::make_unique<entt::DefaultRegistry>();

            mDynamics = std::make_unique<physics::Dynamics>(nullptr, getTickFrequency());
            mTileWorld = std::make_unique<entities::TileWorld>();

            mServer = std::make_unique<network::Server>(&address, 16, 2, *mForegroundEntities, *mDynamics);

            loadLevel();
        }

        ServerGame::~ServerGame() = default;

        void ServerGame::loadLevel() {
            auto boxSize = math::vec2{4.0f, 1.0f};
            auto boxPositionInWorld = math::vec3{-0.5f, -2.5f, 1.0f};

            std::string boxTexturePath = "resources/images/box/1/box.png";
            auto boxTexture = components::Texture{};
            boxTexture.filePath = boxTexturePath;
            boxTexture.status = components::TextureStatus::NEEDS_LOADING_USING_PATH;
            mBoxEntity = entities::createStaticPhysicsEntity(*mForegroundEntities,
                                                             *mDynamics->getPhysicsWorld(), boxSize,
                                                             boxPositionInWorld);
            entities::assignTexture(*mForegroundEntities, mBoxEntity, boxTexture);

            auto truckSize = math::vec2{1.0f, 3.0f};
            auto truckPositionInWorld = math::vec3{-0.5f, 5.0f, 1.0f};
            std::string truckTexturePath = "resources/images/car/2/truck.png";
            auto truckTexture = components::Texture{};
            truckTexture.filePath = truckTexturePath;
            truckTexture.status = components::TextureStatus::NEEDS_LOADING_USING_PATH;
            mTruckEntity = entities::createDynamicPhysicsEntity(*mForegroundEntities,
                                                                *mDynamics->getPhysicsWorld(), truckSize,
                                                                truckPositionInWorld, 0.0f,
                                                                math::vec3{1.0f, 1.0f, 0.0f});
            entities::assignTexture(*mForegroundEntities, mTruckEntity, truckTexture);
        }

        bool ServerGame::shouldTerminate() {
            return false;
        }

        void ServerGame::_tick(const common::real32 tickTime) {
            mServer->poll();

            // Fake lag
            //std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 4));

            for (auto client: mServer->getClients()) {
                auto input = mServer->getClientInput(client);
                mDynamics->tick(*mForegroundEntities, input, tickTime);

                auto carEntity = mServer->getCarEntity(client);
                auto car = mForegroundEntities->get<components::Car>(carEntity);
                auto carPlacement = mForegroundEntities->get<components::Placement>(carEntity);
                mTileWorld->tick(carPlacement.position.getXY(), car, *mForegroundEntities, *mBackgroundEntities);
            }

            mServer->sendWorldData(*mForegroundEntities);
        }

        void ServerGame::_display() {}

        void ServerGame::_render() {}

        void ServerGame::showFPS(int16 fps) {}

        void ServerGame::showTPS(int16 tps) {}

        void ServerGame::showFET(common::int16 fet) {}
    }
}
