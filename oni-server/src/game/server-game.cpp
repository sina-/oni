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

            mServer = std::make_unique<network::Server>(&address, 16, 2);

            loadLevel();
        }

        ServerGame::~ServerGame() = default;

        void ServerGame::loadLevel() {
            mCarEntity = entities::createVehicleEntity(*mForegroundEntities, *mDynamics->getPhysicsWorld());

            // IMPORTANT NOTE: Newbie trap! carConfig must be a copy, otherwise createEntity calls will resize the data
            // storage and the old reference will be invalidated and then we end-up getting garbage :(
            auto carConfig = mForegroundEntities->get<components::CarConfig>(mCarEntity);

            auto tireRotation = static_cast<common::real32>(math::toRadians(90.0f));

            auto tireSize = math::vec2{};
            tireSize.x = static_cast<common::real32>(carConfig.wheelWidth);
            tireSize.y = static_cast<common::real32>(carConfig.wheelRadius * 2);

            auto tireFRPos = math::vec3{};
            tireFRPos.x = static_cast<common::real32>(carConfig.cgToFrontAxle - carConfig.wheelRadius);
            tireFRPos.y = static_cast<common::real32>(carConfig.halfWidth / 2 + carConfig.wheelWidth);
            auto carTireFREntity = entities::createDynamicEntity(*mForegroundEntities, tireSize, tireFRPos,
                                                                 tireRotation, math::vec3{1.0f, 1.0f, 0.0f});
            entities::TransformationHierarchy::createTransformationHierarchy(*mForegroundEntities, mCarEntity,
                                                                             carTireFREntity);

            auto tireFLPos = math::vec3{};
            tireFLPos.x = static_cast<common::real32>(carConfig.cgToFrontAxle - carConfig.wheelRadius);
            tireFLPos.y = static_cast<common::real32>(-carConfig.halfWidth / 2 - carConfig.wheelWidth);
            auto carTireFLEntity = entities::createDynamicEntity(*mForegroundEntities, tireSize, tireFLPos,
                                                                 tireRotation, math::vec3{1.0f, 1.0f, 0.0f});
            entities::TransformationHierarchy::createTransformationHierarchy(*mForegroundEntities, mCarEntity,
                                                                             carTireFLEntity);

            auto tireRRPos = math::vec3{};
            tireRRPos.x = static_cast<common::real32>(-carConfig.cgToRearAxle);
            tireRRPos.y = static_cast<common::real32>(carConfig.halfWidth / 2 + carConfig.wheelWidth);
            auto carTireRREntity = entities::createDynamicEntity(*mForegroundEntities, tireSize, tireRRPos,
                                                                 tireRotation, math::vec3{1.0f, 1.0f, 0.0f});
            entities::TransformationHierarchy::createTransformationHierarchy(*mForegroundEntities, mCarEntity,
                                                                             carTireRREntity);

            auto tireRLPos = math::vec3{};
            tireRLPos.x = static_cast<common::real32>(-carConfig.cgToRearAxle);
            tireRLPos.y = static_cast<common::real32>(-carConfig.halfWidth / 2 - carConfig.wheelWidth);
            auto carTireRLEntity = entities::createDynamicEntity(*mForegroundEntities, tireSize, tireRLPos,
                                                                 tireRotation, math::vec3{1.0f, 1.0f, 0.0f});
            entities::TransformationHierarchy::createTransformationHierarchy(*mForegroundEntities, mCarEntity,
                                                                             carTireRLEntity);

            auto &car = mForegroundEntities->get<components::Car>(mCarEntity);
            car.tireFR = carTireFREntity;
            car.tireFL = carTireFLEntity;
            car.tireRR = carTireRREntity;
            car.tireRL = carTireRLEntity;

            auto boxSize = math::vec2{4.0f, 1.0f};
            auto boxPositionInWorld = math::vec3{-0.5f, -2.5f, 1.0f};

            mBoxEntity = entities::createStaticPhysicsEntity(*mForegroundEntities,
                                                             *mDynamics->getPhysicsWorld(), boxSize,
                                                             boxPositionInWorld);

            auto truckSize = math::vec2{1.0f, 3.0f};
            auto truckPositionInWorld = math::vec3{-0.5f, 5.0f, 1.0f};
            mTruckEntity = entities::createDynamicPhysicsEntity(*mForegroundEntities,
                                                                *mDynamics->getPhysicsWorld(), truckSize,
                                                                truckPositionInWorld, 0.0f,
                                                                math::vec3{1.0f, 1.0f, 0.0f});
        }

        bool ServerGame::shouldTerminate() {
            return false;
        }

        void ServerGame::_tick(const common::real32 tickTime) {
            mServer->poll();
            //mDynamics->tick(*mForegroundEntities, *mInput, tickTime);

            // Fake lag
            //std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 4));

            auto car = mForegroundEntities->get<components::Car>(mCarEntity);

            auto carPlacement = mForegroundEntities->get<components::Placement>(mCarEntity);

            mTileWorld->tick(carPlacement.position.getXY(), car, *mForegroundEntities, *mBackgroundEntities);
        }

        void ServerGame::_display() {}

        void ServerGame::_render() {}

        void ServerGame::showFPS(int16 fps) {}

        void ServerGame::showTPS(int16 tps) {}

        void ServerGame::showFET(common::int16 fet) {}
    }
}
