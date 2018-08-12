#include <oni-server/game/server-game.h>

#include <thread>

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
            mEntityManager = std::make_unique<entities::EntityManager>();

            mDynamics = std::make_unique<physics::Dynamics>(nullptr, getTickFrequency());
            mTileWorld = std::make_unique<entities::TileWorld>();

            mServer = std::make_unique<network::Server>(&address, 16, 2);

            mServer->registerPacketHandler(network::PacketType::SETUP_SESSION,
                                           std::bind(&ServerGame::setupSessionPacketHandler, this,
                                                     std::placeholders::_1, std::placeholders::_2));
            mServer->registerPacketHandler(network::PacketType::CLIENT_INPUT,
                                           std::bind(&ServerGame::clientInputPacketHandler, this, std::placeholders::_1,
                                                     std::placeholders::_2));

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
            mBoxEntity = entities::createStaticPhysicsEntity(*mEntityManager,
                                                             *mDynamics->getPhysicsWorld(), boxSize,
                                                             boxPositionInWorld);
            mEntityManager->assign<components::Texture>(mBoxEntity, boxTexture);

            auto truckSize = math::vec2{1.0f, 3.0f};
            auto truckPositionInWorld = math::vec3{-0.5f, 5.0f, 1.0f};
            std::string truckTexturePath = "resources/images/car/2/truck.png";
            auto truckTexture = components::Texture{};
            truckTexture.filePath = truckTexturePath;
            truckTexture.status = components::TextureStatus::NEEDS_LOADING_USING_PATH;
            mTruckEntity = entities::createDynamicPhysicsEntity(*mEntityManager,
                                                                *mDynamics->getPhysicsWorld(), truckSize,
                                                                truckPositionInWorld, 0.0f,
                                                                math::vec3{1.0f, 1.0f, 0.0f});
            mEntityManager->assign<components::Texture>(mTruckEntity, truckTexture);
        }

        void ServerGame::setupSessionPacketHandler(network::PeerID clientID, const std::string &data) {
            auto carEntity = entities::createVehicleEntity(*mEntityManager, *mDynamics->getPhysicsWorld());

            auto carTexture = components::Texture{};
            std::string carTexturePath = "resources/images/car/1/car.png";
            carTexture.filePath = carTexturePath;
            carTexture.status = components::TextureStatus::NEEDS_LOADING_USING_PATH;
            mEntityManager->assign<components::Texture>(carEntity, carTexture);

            // IMPORTANT NOTE: Newbie trap! carConfig must be a copy, otherwise createEntity calls will resize the data
            // storage and the old reference will be invalidated and then we end-up getting garbage :(
            auto carConfig = mEntityManager->get<components::CarConfig>(carEntity);

            std::string carTireTexturePath = "resources/images/car/1/car-tire.png";
            auto carTireTexture = components::Texture{};
            carTireTexture.filePath = carTireTexturePath;
            carTireTexture.status = components::TextureStatus::NEEDS_LOADING_USING_PATH;

            auto tireRotation = static_cast<common::real32>(math::toRadians(90.0f));

            auto tireSize = math::vec2{};
            tireSize.x = static_cast<common::real32>(carConfig.wheelWidth);
            tireSize.y = static_cast<common::real32>(carConfig.wheelRadius * 2);

            auto tireFRPos = math::vec3{};
            tireFRPos.x = static_cast<common::real32>(carConfig.cgToFrontAxle - carConfig.wheelRadius);
            tireFRPos.y = static_cast<common::real32>(carConfig.halfWidth / 2 + carConfig.wheelWidth);
            auto carTireFREntity = entities::createDynamicEntity(*mEntityManager, tireSize, tireFRPos,
                                                                 tireRotation, math::vec3{1.0f, 1.0f, 0.0f});
            entities::TransformationHierarchy::createTransformationHierarchy(*mEntityManager, carEntity,
                                                                             carTireFREntity);
            mEntityManager->assign<components::Texture>(carTireFREntity, carTireTexture);

            auto tireFLPos = math::vec3{};
            tireFLPos.x = static_cast<common::real32>(carConfig.cgToFrontAxle - carConfig.wheelRadius);
            tireFLPos.y = static_cast<common::real32>(-carConfig.halfWidth / 2 - carConfig.wheelWidth);
            auto carTireFLEntity = entities::createDynamicEntity(*mEntityManager, tireSize, tireFLPos,
                                                                 tireRotation, math::vec3{1.0f, 1.0f, 0.0f});
            entities::TransformationHierarchy::createTransformationHierarchy(*mEntityManager, carEntity,
                                                                             carTireFLEntity);
            mEntityManager->assign<components::Texture>(carTireFLEntity, carTireTexture);

            auto tireRRPos = math::vec3{};
            tireRRPos.x = static_cast<common::real32>(-carConfig.cgToRearAxle);
            tireRRPos.y = static_cast<common::real32>(carConfig.halfWidth / 2 + carConfig.wheelWidth);
            auto carTireRREntity = entities::createDynamicEntity(*mEntityManager, tireSize, tireRRPos,
                                                                 tireRotation, math::vec3{1.0f, 1.0f, 0.0f});
            entities::TransformationHierarchy::createTransformationHierarchy(*mEntityManager, carEntity,
                                                                             carTireRREntity);
            mEntityManager->assign<components::Texture>(carTireRREntity, carTireTexture);

            auto tireRLPos = math::vec3{};
            tireRLPos.x = static_cast<common::real32>(-carConfig.cgToRearAxle);
            tireRLPos.y = static_cast<common::real32>(-carConfig.halfWidth / 2 - carConfig.wheelWidth);
            auto carTireRLEntity = entities::createDynamicEntity(*mEntityManager, tireSize, tireRLPos,
                                                                 tireRotation, math::vec3{1.0f, 1.0f, 0.0f});
            entities::TransformationHierarchy::createTransformationHierarchy(*mEntityManager, carEntity,
                                                                             carTireRLEntity);
            mEntityManager->assign<components::Texture>(carTireRLEntity, carTireTexture);

            auto &car = mEntityManager->get<components::Car>(carEntity);
            car.tireFR = carTireFREntity;
            car.tireFL = carTireFLEntity;
            car.tireRR = carTireRREntity;
            car.tireRL = carTireRLEntity;

            mClientCarEntityMap[clientID] = carEntity;

            mServer->sendCarEntityID(carEntity, clientID);
        }

        void ServerGame::clientInputPacketHandler(network::PeerID clientID, const std::string &data) {
            auto input = network::deserialize<io::Input>(data);
            // TODO: Avoid copy by using a unique_ptr or something
            mClientInputMap[clientID] = input;
        }

        bool ServerGame::shouldTerminate() {
            return false;
        }

        void ServerGame::_tick(const common::real32 tickTime) {
            mServer->poll();

            // Fake lag
            //std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 4));

            for (auto client: mServer->getClients()) {
                auto input = mClientInputMap[client];
                mDynamics->tick(*mEntityManager, input, tickTime);

                auto carEntity = mClientCarEntityMap[client];
                auto car = mEntityManager->get<components::Car>(carEntity);
                const auto &carPlacement = mEntityManager->get<components::Placement>(carEntity);
                mTileWorld->tick(*mEntityManager, car, carPlacement.position.getXY());
            }

            mServer->sendEntities(*mEntityManager);
        }

        void ServerGame::_poll() {

        }

        void ServerGame::_display() {}

        void ServerGame::_render() {}

        void ServerGame::showFPS(int16 fps) {}

        void ServerGame::showTPS(int16 tps) {}

        void ServerGame::showFET(common::int16 fet) {}

    }
}
