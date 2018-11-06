#include <oni-server/game/server-game.h>

#include <thread>

#include <oni-core/entities/create-entity.h>
#include <oni-core/io/output.h>
#include <oni-core/audio/audio-manager-fmod.h>
#include <oni-core/physics/transformation.h>
#include <oni-core/components/visual.h>
#include <oni-core/common/consts.h>
#include <oni-core/entities/manage-hierarchy.h>
#include <oni-core/entities/serialization.h>
#include <oni-core/network/server.h>
#include <oni-core/io/input.h>


namespace oni {
    namespace server {
        ServerGame::ServerGame(const network::Address &address) : Game(), mServerAddress(address) {
            srand(static_cast<unsigned int>(time(nullptr)));
            mEntityManager = std::make_unique<entities::EntityManager>();

            mDynamics = std::make_unique<physics::Dynamics>(getTickFrequency());
            // TODO: Passing reference to unique_ptr and also exposing the b2World into the other classes!
            // Maybe I can expose subset of functionalities I need from Dynamics class, maybe even better to call it
            // physics class part of which is dynamics.
            mTileWorld = std::make_unique<entities::TileWorld>(*mEntityManager, *mDynamics->getPhysicsWorld());
            mClientDataManager = std::make_unique<entities::ClientDataManager>();

            mServer = std::make_unique<network::Server>(&address, 16, 2);

            mServer->registerPacketHandler(network::PacketType::SETUP_SESSION,
                                           std::bind(&ServerGame::setupSessionPacketHandler, this,
                                                     std::placeholders::_1, std::placeholders::_2));
            mServer->registerPacketHandler(network::PacketType::CLIENT_INPUT,
                                           std::bind(&ServerGame::clientInputPacketHandler, this, std::placeholders::_1,
                                                     std::placeholders::_2));

            mServer->registerPostDisconnectHook(std::bind(&ServerGame::postDisconnectHook, this,
                                                          std::placeholders::_1));

            loadLevel();
        }

        ServerGame::~ServerGame() = default;

        void ServerGame::loadLevel() {
            /*
            auto boxSize = math::vec2{4.0f, 1.0f};
            auto boxPositionInWorld = math::vec3{-0.5f, -2.5f, 1.0f};
            std::string boxTexturePath = "resources/images/box/1/box.png";
            auto boxTexture = components::Texture{};
            boxTexture.filePath = boxTexturePath;
            boxTexture.status = components::TextureStatus::NEEDS_LOADING_USING_PATH;
            mBoxEntity = entities::createStaticPhysicsEntity(*mEntityManager,
                                                             *mDynamics->getPhysicsWorld(), boxSize,
                                                             boxPositionInWorld);
            entities::assignTexture(*mEntityManager, mBoxEntity, boxTexture);
            */

            auto truckSize = math::vec2{1.0f, 3.0f};
            auto truckPositionInWorld = math::vec3{-60.0f, -30.0f, 1.0f};
            std::string truckTexturePath = "resources/images/car/2/truck.png";
            auto truckTexture = components::Texture{};
            truckTexture.filePath = truckTexturePath;
            truckTexture.status = components::TextureStatus::NEEDS_LOADING_USING_PATH;
            mTruckEntity = entities::createDynamicPhysicsEntity(*mEntityManager,
                                                                *mDynamics->getPhysicsWorld(), truckSize,
                                                                truckPositionInWorld, 0.0f,
                                                                math::vec3{1.0f, 1.0f, 0.0f});
            entities::assignTexture(*mEntityManager, mTruckEntity, truckTexture);

        }

        void ServerGame::setupSessionPacketHandler(const common::PeerID &clientID, const std::string &data) {
            auto carConfig = components::CarConfig();
            carConfig.cgToRear = 1.25f;
            carConfig.cgToFront = 1.25f;
            carConfig.cgToFrontAxle = 1.15f;
            carConfig.cgToRearAxle = 1.00f;
            carConfig.halfWidth = 0.55f;
            carConfig.inertialScale = 0.5f;
            carConfig.lockGrip = 0.2f;
            carConfig.tireGrip = 3.0f;
            carConfig.engineForce = 10000;
            carConfig.brakeForce = 4000;
            carConfig.cornerStiffnessRear = 5.5f;
            carConfig.cornerStiffnessFront = 5.0f;
            carConfig.maxSteer = 0.5f;
            carConfig.rollResist = 8.0f;
            carConfig.wheelRadius = 0.25f;

            auto carEntity = entities::createVehicleEntity(*mEntityManager, *mDynamics->getPhysicsWorld(),
                                                           carConfig);

            auto carTexture = components::Texture{};
            std::string carTexturePath = "resources/images/car/1/car.png";
            carTexture.filePath = carTexturePath;
            carTexture.status = components::TextureStatus::NEEDS_LOADING_USING_PATH;
            entities::assignTexture(*mEntityManager, carEntity, carTexture);

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
            entities::assignTexture(*mEntityManager, carTireFREntity, carTireTexture);

            auto tireFLPos = math::vec3{};
            tireFLPos.x = static_cast<common::real32>(carConfig.cgToFrontAxle - carConfig.wheelRadius);
            tireFLPos.y = static_cast<common::real32>(-carConfig.halfWidth / 2 - carConfig.wheelWidth);
            auto carTireFLEntity = entities::createDynamicEntity(*mEntityManager, tireSize, tireFLPos,
                                                                 tireRotation, math::vec3{1.0f, 1.0f, 0.0f});
            entities::TransformationHierarchy::createTransformationHierarchy(*mEntityManager, carEntity,
                                                                             carTireFLEntity);
            entities::assignTexture(*mEntityManager, carTireFLEntity, carTireTexture);

            auto tireRRPos = math::vec3{};
            tireRRPos.x = static_cast<common::real32>(-carConfig.cgToRearAxle);
            tireRRPos.y = static_cast<common::real32>(carConfig.halfWidth / 2 + carConfig.wheelWidth);
            auto carTireRREntity = entities::createDynamicEntity(*mEntityManager, tireSize, tireRRPos,
                                                                 tireRotation, math::vec3{1.0f, 1.0f, 0.0f});
            entities::TransformationHierarchy::createTransformationHierarchy(*mEntityManager, carEntity,
                                                                             carTireRREntity);
            entities::assignTexture(*mEntityManager, carTireRREntity, carTireTexture);

            auto tireRLPos = math::vec3{};
            tireRLPos.x = static_cast<common::real32>(-carConfig.cgToRearAxle);
            tireRLPos.y = static_cast<common::real32>(-carConfig.halfWidth / 2 - carConfig.wheelWidth);
            auto carTireRLEntity = entities::createDynamicEntity(*mEntityManager, tireSize, tireRLPos,
                                                                 tireRotation, math::vec3{1.0f, 1.0f, 0.0f});
            entities::TransformationHierarchy::createTransformationHierarchy(*mEntityManager, carEntity,
                                                                             carTireRLEntity);
            entities::assignTexture(*mEntityManager, carTireRLEntity, carTireTexture);

            // NOTE: There is no need to lock the registry as no system is using this entity
            auto &car = mEntityManager->get<components::Car>(carEntity);
            car.tireFR = carTireFREntity;
            car.tireFL = carTireFLEntity;
            car.tireRR = carTireRREntity;
            car.tireRL = carTireRLEntity;

            mServer->sendCarEntityID(carEntity, clientID);
            mServer->sendEntitiesAll(*mEntityManager);

            mClientDataManager->addNewClient(clientID, carEntity);
        }

        void ServerGame::clientInputPacketHandler(const common::PeerID &clientID, const std::string &data) {
            auto input = network::deserialize<io::Input>(data);
            // TODO: Avoid copy by using a unique_ptr or something
            mClientDataManager->setClientInput(clientID, input);
        }

        void ServerGame::postDisconnectHook(const common::PeerID &peerID) {
            auto clientDataLock = mClientDataManager->scopedLock();
            auto clientCarEntityID = mClientDataManager->getEntityID(peerID);
            entities::deleteVehicleEntity(*mEntityManager, *mDynamics->getPhysicsWorld(), clientCarEntityID);
            mClientDataManager->deleteClient(peerID);
        }

        bool ServerGame::shouldTerminate() {
            return false;
        }

        void ServerGame::_poll() {
            mServer->poll();

            if (mClientDataManager->getNumClients()) {
                mServer->sendComponentsUpdate(*mEntityManager);
                mServer->sendNewEntities(*mEntityManager);

                bool needToSendDeleted = false;
                {
                    auto lock = mEntityManager->scopedLock();
                    needToSendDeleted = mEntityManager->numberOfDeletedEntities() > 0;
                }
                if (needToSendDeleted) {
                    mServer->sendRemainingEntitiesAfterDelete(*mEntityManager);
                    {
                        auto lock = mEntityManager->scopedLock();
                        // TODO: What happens if broadcast fails for some clients? Would they miss these entities forever?
                        mEntityManager->clearDeletedEntitiesList();
                    }
                }
            }
        }

        void ServerGame::_sim(const common::real64 tickTime) {
            // Fake lag
            //std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 4));

            std::vector<io::Input> clientInput{};
            {
                auto clientDataLock = mClientDataManager->scopedLock();
                clientInput = mClientDataManager->getClientsInput();
                // TODO: Maybe I should just stack up client input and remove them after the processing is done.
            }

            for (const auto &input: clientInput) {
                mDynamics->tick(*mEntityManager, input, tickTime);
            }

            std::vector<math::vec2> tickPositions{};
            {
                // TODO: This is just awful :( so many locks. Accessing entities from registry without a view, which
                // is slow and by the time the positions are passed to other systems, such as TileWorld, the entities
                // might not even be there anymore. It won't crash because registry will be locked, but then what is the
                // point of locking? Maybe I should drop this whole multi-thread everything shenanigans and just do
                // things in sequence but have a pool of workers that can do parallel shit on demand for heavy lifting.
                auto registryLock = mEntityManager->scopedLock();
                auto clientDataLock = mClientDataManager->scopedLock();
                for (const auto &carEntity: mClientDataManager->getCarEntities()) {
                    const auto &carPlacement = mEntityManager->get<components::Placement>(carEntity);
                    tickPositions.push_back(carPlacement.position.getXY());
                }
            }

            for (const auto &pos: tickPositions) {
                mTileWorld->tick(pos);
            }
        }

        void ServerGame::_render() {}

        void ServerGame::_display() {}

        void ServerGame::showFPS(int16 fps) {}

        void ServerGame::showSPS(oni::common::int16 tps) {}

        void ServerGame::showPPS(oni::common::int16 pps) {
            std::cout << "PPS " << pps << "\n";
        }

        void ServerGame::showPET(oni::common::int16 pet) {}

        void ServerGame::showSET(oni::common::int16 set) {}

        void ServerGame::showRET(oni::common::int16 ret) {}
    }
}
