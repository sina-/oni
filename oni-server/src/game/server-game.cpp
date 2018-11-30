#include <oni-server/game/server-game.h>

#include <thread>

#include <oni-core/components/visual.h>
#include <oni-core/common/consts.h>
#include <oni-core/gameplay/lap-tracker.h>
#include <oni-core/physics/dynamics.h>
#include <oni-core/network/server.h>
#include <oni-core/entities/create-entity.h>
#include <oni-core/entities/manage-hierarchy.h>
#include <oni-core/entities/client-data-manager.h>
#include <oni-core/entities/entity-manager.h>
#include <oni-core/entities/tile-world.h>
#include <oni-core/entities/serialization.h>


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
            mLapTracker = std::make_unique<gameplay::LapTracker>(*mEntityManager);

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

            mTruckEntity = createTruck();
        }

        void ServerGame::setupSessionPacketHandler(const common::PeerID &clientID, const std::string &data) {
            auto carEntity = createCar();

            mServer->sendCarEntityID(carEntity, clientID);
            mServer->sendEntitiesAll(*mEntityManager);

            mClientDataManager->addNewClient(clientID, carEntity);
        }

        void ServerGame::clientInputPacketHandler(const common::PeerID &clientID, const std::string &data) {
            auto input = entities::deserialize<io::Input>(data);
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
                // point of multi threading? Maybe I should drop this whole multi-thread everything shenanigans and just do
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

            mLapTracker->tick();
        }

        void ServerGame::_render() {}

        void ServerGame::_display() {}

        void ServerGame::showFPS(oni::common::int16 fps) {}

        void ServerGame::showSPS(oni::common::int16 tps) {}

        void ServerGame::showPPS(oni::common::int16 pps) {
            std::cout << "PPS " << pps << "\n";
        }

        void ServerGame::showPET(oni::common::int16 pet) {}

        void ServerGame::showSET(oni::common::int16 set) {}

        void ServerGame::showRET(oni::common::int16 ret) {}

        common::EntityID ServerGame::createCar() {
            auto lock = mEntityManager->scopedLock();

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

            // TODO: All cars spawn in the same location!
            auto carPosition = math::vec3{-70.f, -30.f, 1.f};

            auto carSizeX = carConfig.cgToRear + carConfig.cgToFront;
            auto carSizeY = carConfig.halfWidth * 2.0f;
            auto carSize = math::vec2{static_cast<common::real32>(carSizeX), static_cast<common::real32>(carSizeY)};
            assert(carSizeX - carConfig.cgToFront - carConfig.cgToRear < 0.00001f);

            std::string carTexturePath = "resources/images/car/1/car.png";

            auto carEntityID = entities::createEntity(*mEntityManager);
            entities::assignPhysicalProperties(*mEntityManager, *mDynamics->getPhysicsWorld(), carEntityID, carPosition,
                                               carSize,
                                               components::BodyType::DYNAMIC, true);
            entities::assignShapeLocal(*mEntityManager, carEntityID, carSize);
            entities::assignPlacement(*mEntityManager, carEntityID, carPosition, math::vec3{1.f, 1.f, 0.f}, 0.f);
            entities::assignCar(*mEntityManager, carEntityID, carPosition, carConfig);
            entities::assignTextureToLoad(*mEntityManager, carEntityID, carTexturePath);
            entities::assignTag<components::Tag_Dynamic>(*mEntityManager, carEntityID);
            entities::assignTag<components::Tag_Vehicle>(*mEntityManager, carEntityID);

            math::vec2 tireSize;
            tireSize.x = static_cast<common::real32>(carConfig.wheelWidth);
            tireSize.y = static_cast<common::real32>(carConfig.wheelRadius * 2);

            math::vec3 tireFRPos;
            tireFRPos.x = static_cast<common::real32>(carConfig.cgToFrontAxle - carConfig.wheelRadius);
            tireFRPos.y = static_cast<common::real32>(carConfig.halfWidth / 2 + carConfig.wheelWidth);
            auto carTireFREntity = createTire(carEntityID, tireFRPos, tireSize);

            math::vec3 tireFLPos;
            tireFLPos.x = static_cast<common::real32>(carConfig.cgToFrontAxle - carConfig.wheelRadius);
            tireFLPos.y = static_cast<common::real32>(-carConfig.halfWidth / 2 - carConfig.wheelWidth);
            auto carTireFLEntity = createTire(carEntityID, tireFLPos, tireSize);

            math::vec3 tireRRPos;
            tireRRPos.x = static_cast<common::real32>(-carConfig.cgToRearAxle);
            tireRRPos.y = static_cast<common::real32>(carConfig.halfWidth / 2 + carConfig.wheelWidth);
            auto carTireRREntity = createTire(carEntityID, tireRRPos, tireSize);

            math::vec3 tireRLPos;
            tireRLPos.x = static_cast<common::real32>(-carConfig.cgToRearAxle);
            tireRLPos.y = static_cast<common::real32>(-carConfig.halfWidth / 2 - carConfig.wheelWidth);
            auto carTireRLEntity = createTire(carEntityID, tireRLPos, tireSize);

            auto &car = mEntityManager->get<components::Car>(carEntityID);
            car.tireFR = carTireFREntity;
            car.tireFL = carTireFLEntity;
            car.tireRR = carTireRREntity;
            car.tireRL = carTireRLEntity;

            return carEntityID;
        }

        common::EntityID ServerGame::createTire(common::EntityID carEntityID,
                                                const math::vec3 &pos,
                                                const math::vec2 &size) {
            auto heading = static_cast<common::real32>(math::toRadians(90.0f));
            math::vec3 scale{1.f, 1.f, 1.f};
            std::string carTireTexturePath = "resources/images/car/1/car-tire.png";

            auto entityID = createEntity(*mEntityManager);
            entities::assignShapeLocal(*mEntityManager, entityID, size);
            entities::assignPlacement(*mEntityManager, entityID, pos, scale, heading);
            entities::assignTextureToLoad(*mEntityManager, entityID, carTireTexturePath);
            entities::assignTag<components::Tag_Dynamic>(*mEntityManager, entityID);

            entities::TransformationHierarchy::createTransformationHierarchy(*mEntityManager, carEntityID, entityID);
            return entityID;
        }

        common::EntityID ServerGame::createTruck() {
            math::vec2 size{1.0f, 3.0f};
            math::vec3 worldPos{-60.0f, -30.0f, 1.0f};
            std::string truckTexturePath = "resources/images/car/2/truck.png";

            auto lock = mEntityManager->scopedLock();
            auto entityID = entities::createEntity(*mEntityManager);
            entities::assignShapeLocal(*mEntityManager, entityID, size);
            entities::assignPlacement(*mEntityManager, entityID, worldPos, {1.f, 1.f, 0.f}, 0.f);
            entities::assignPhysicalProperties(*mEntityManager, *mDynamics->getPhysicsWorld(), entityID, worldPos, size,
                                               components::BodyType::DYNAMIC, false);
            entities::assignTextureToLoad(*mEntityManager, entityID, truckTexturePath);
            entities::assignTag<components::Tag_Dynamic>(*mEntityManager, entityID);

            return entityID;
        }
    }
}
