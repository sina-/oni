#include <oni-server/game/server-game.h>

#include <thread>

#include <oni-core/math/z-layer-manager.h>
#include <oni-core/component/visual.h>
#include <oni-core/common/consts.h>
#include <oni-core/gameplay/lap-tracker.h>
#include <oni-core/physics/dynamics.h>
#include <oni-core/network/server.h>
#include <oni-core/entities/create-entity.h>
#include <oni-core/entities/client-data-manager.h>
#include <oni-core/entities/entity-manager.h>
#include <oni-core/entities/serialization.h>
#include <oni-core/physics/transformation.h>

#include <oni-server/entities/tile-world.h>


namespace oni {
    namespace server {
        namespace game {
            ServerGame::ServerGame(const oni::network::Address &address) : Game(), mServerAddress(address) {
                srand(static_cast<unsigned int>(time(nullptr)));

                mZLayerManager = std::make_unique<oni::math::ZLayerManager>();
                mEntityManager = std::make_unique<oni::entities::EntityManager>();

                mDynamics = std::make_unique<oni::physics::Dynamics>(getTickFrequency());
                // TODO: Passing reference to unique_ptr and also exposing the b2World into the other classes!
                // Maybe I can expose subset of functionalities I need from Dynamics class, maybe even better to call it
                // physics class part of which is dynamics.
                mTileWorld = std::make_unique<oni::server::entities::TileWorld>(*mEntityManager,
                                                                                *mDynamics->getPhysicsWorld(),
                                                                                *mZLayerManager);

                mClientDataManager = std::make_unique<oni::entities::ClientDataManager>();
                mLapTracker = std::make_unique<oni::gameplay::LapTracker>(*mEntityManager, *mZLayerManager);

                mServer = std::make_unique<oni::network::Server>(&address, 16, 2);

                mServer->registerPacketHandler(oni::network::PacketType::SETUP_SESSION,
                                               std::bind(&ServerGame::setupSessionPacketHandler, this,
                                                         std::placeholders::_1, std::placeholders::_2));
                mServer->registerPacketHandler(oni::network::PacketType::CLIENT_INPUT,
                                               std::bind(&ServerGame::clientInputPacketHandler, this,
                                                         std::placeholders::_1,
                                                         std::placeholders::_2));
                mServer->registerPacketHandler(oni::network::PacketType::Z_LAYER,
                                               std::bind(&ServerGame::zLayerPacketHandler, this,
                                                         std::placeholders::_1,
                                                         std::placeholders::_2));

                mServer->registerPostDisconnectHook(std::bind(&ServerGame::postDisconnectHook, this,
                                                              std::placeholders::_1));

                loadLevel();
            }

            ServerGame::~ServerGame() = default;

            void ServerGame::loadLevel() {
                mTruckEntity = createTruck();

                mTileWorld->generateDemoRaceCourse();

/*            {
                auto lock = mEntityManager->scopedLock();
                auto box1 = entities::createEntity(*mEntityManager);
                auto box2 = entities::createEntity(*mEntityManager);
                auto boxSize = math::vec2{4.0f, 1.0f};
                auto box1PosInWorld = math::vec3{-70.f, -25.f, 0.5f};
                auto box2PosInWorld = math::vec3{-70.f, -25.f, 0.4f};
                std::string boxTexturePath = "resources/images/box/1/box.png";

                entities::assignShapeWorld(*mEntityManager, box1, boxSize, box1PosInWorld);
                entities::assignTextureToLoad(*mEntityManager, box1, boxTexturePath);
                entities::assignTag<components::Tag_Static>(*mEntityManager, box1);

                entities::assignShapeLocal(*mEntityManager, box2, boxSize);
                entities::assignTextureToLoad(*mEntityManager, box2, boxTexturePath);
                entities::assignPlacement(*mEntityManager, box2, box2PosInWorld, {1.f, 1.f, 0.f}, math::toRadians(90.f));
                entities::assignTag<components::Tag_Dynamic>(*mEntityManager, box2);
            }*/
            }

            void ServerGame::setupSessionPacketHandler(const oni::common::PeerID &clientID, const std::string &data) {
                auto carEntity = createCar();

                mServer->sendCarEntityID(carEntity, clientID);
                mServer->sendEntitiesAll(*mEntityManager);

                auto lock = mClientDataManager->scopedLock();
                mClientDataManager->addNewClient(clientID, carEntity);
            }

            void ServerGame::clientInputPacketHandler(const oni::common::PeerID &clientID, const std::string &data) {
                auto input = oni::entities::deserialize<oni::io::Input>(data);
                // TODO: Avoid copy by using a unique_ptr or something
                auto lock = mClientDataManager->scopedLock();
                mClientDataManager->setClientInput(clientID, input);
            }

            void ServerGame::zLayerPacketHandler(const oni::common::PeerID &clientID, const std::string &data) {
                mServer->sendZLayer(clientID, mZLayerManager->getZLayer());
            }

            void ServerGame::postDisconnectHook(const oni::common::PeerID &peerID) {
                auto clientDataLock = mClientDataManager->scopedLock();
                auto clientCarEntityID = mClientDataManager->getEntityID(peerID);

                removeCar(clientCarEntityID);
                mClientDataManager->deleteClient(peerID);
            }

            bool ServerGame::shouldTerminate() {
                return false;
            }

            void ServerGame::_poll() {
                mServer->poll();

                mServer->sendComponentsUpdate(*mEntityManager);
                mServer->sendNewEntities(*mEntityManager);
                mServer->broadcastSpawnParticle(*mEntityManager);

                {
                    auto lock = mEntityManager->scopedLock();
                    if (mEntityManager->containsDeletedEntities()) {
                        mServer->broadcastDeletedEntities(*mEntityManager);
                        // TODO: What happens if broadcast fails for some clients? Would they miss these entities forever?
                        mEntityManager->clearDeletedEntitiesList();
                    }
                }
            }

            void ServerGame::_sim(const oni::common::real64 tickTime) {
                // Fake lag
                //std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 4));

                mDynamics->tick(*mEntityManager, *mClientDataManager, tickTime);

                std::vector<oni::math::vec2> tickPositions{};
                {
                    // TODO: This is just awful :( so many locks. Accessing entities from registry without a view, which
                    // is slow and by the time the positions are passed to other systems, such as TileWorld, the entities
                    // might not even be there anymore. It won't crash because registry will be locked, but then what is the
                    // point of multi threading? Maybe I should drop this whole multi-thread everything shenanigans and just do
                    // things in sequence but have a pool of workers that can do parallel shit on demand for heavy lifting.
                    auto registryLock = mEntityManager->scopedLock();
                    auto clientDataLock = mClientDataManager->scopedLock();
                    for (const auto &carEntity: mClientDataManager->getCarEntities()) {
                        const auto &carPlacement = mEntityManager->get<oni::component::Placement>(carEntity);
                        tickPositions.push_back(carPlacement.position.getXY());
                    }
                }

                for (const auto &pos: tickPositions) {
                    mTileWorld->tick(pos);
                }

                mLapTracker->tick();
            }

            void ServerGame::_render(oni::common::real64 simTime) {}

            void ServerGame::_display() {}

            void ServerGame::showFPS(oni::common::int16 fps) {}

            void ServerGame::showSPS(oni::common::int16 tps) {}

            void ServerGame::showPPS(oni::common::int16 pps) {
                std::cout << "PPS " << pps << "\n";
            }

            void ServerGame::showPET(oni::common::int16 pet) {}

            void ServerGame::showSET(oni::common::int16 set) {}

            void ServerGame::showRET(oni::common::int16 ret) {}

            oni::common::EntityID ServerGame::createCar() {
                auto lock = mEntityManager->scopedLock();

                auto carConfig = oni::component::CarConfig();
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

                auto vehicleZ = mZLayerManager->getZForEntity(component::EntityType::RACE_CAR);

                // TODO: All cars spawn in the same location!
                // NOTE: This is the center of car sprite.
                auto carPosition = oni::math::vec3{-70.f, -30.f, vehicleZ};

                auto carSizeX = carConfig.cgToRear + carConfig.cgToFront;
                auto carSizeY = carConfig.halfWidth * 2.0f;
                auto carSize = oni::math::vec2{static_cast<oni::common::real32>(carSizeX),
                                               static_cast<oni::common::real32>(carSizeY)};
                assert(carSizeX - carConfig.cgToFront - carConfig.cgToRear < 0.00001f);
                common::real32 heading = 0.f;

                std::string carTexturePath = "resources/images/car/1/car.png";

                oni::component::PhysicalProperties properties;
                properties.friction = 1.f;
                properties.density = 0.1f;
                properties.angularDamping = 2.f;
                properties.linearDamping = 2.f;
                properties.highPrecision = true;
                properties.bodyType = oni::component::BodyType::DYNAMIC;
                properties.physicalCategory = oni::component::PhysicalCategory::RACE_CAR;

                auto carEntityID = oni::entities::createEntity(*mEntityManager);
                oni::entities::assignPhysicalProperties(*mEntityManager, *mDynamics->getPhysicsWorld(), carEntityID,
                                                        carPosition,
                                                        carSize,
                                                        heading,
                                                        properties);
                oni::entities::assignShapeLocal(*mEntityManager, carEntityID, carSize, vehicleZ);
                oni::entities::assignPlacement(*mEntityManager, carEntityID, carPosition, math::vec3{1.f, 1.f, 0.f},
                                               0.f);
                oni::entities::assignCar(*mEntityManager, carEntityID, carPosition, carConfig);
                oni::entities::assignTextureToLoad(*mEntityManager, carEntityID, carTexturePath);
                oni::entities::assignTag<component::Tag_Dynamic>(*mEntityManager, carEntityID);

                oni::math::vec2 gunSize{};
                gunSize.x = static_cast<common::real32>(carConfig.cgToFrontAxle + carConfig.cgToRearAxle);
                gunSize.y = static_cast<common::real32>(carConfig.halfWidth * 0.8f);

                oni::math::vec3 gunPos{};
                gunPos.x = static_cast<common::real32>(carConfig.cgToFrontAxle * 0.7f);
                gunPos.y = 0.f;
                gunPos.z = mZLayerManager->getZForEntity(component::EntityType::VEHICLE_GUN);
                auto carGunEntity = createGun(carEntityID, gunPos, gunSize);

                oni::math::vec2 tireSize;
                tireSize.x = static_cast<oni::common::real32>(carConfig.wheelWidth);
                tireSize.y = static_cast<oni::common::real32>(carConfig.wheelRadius * 2);

                oni::math::vec3 tireFRPos;
                tireFRPos.x = static_cast<oni::common::real32>(carConfig.cgToFrontAxle - carConfig.wheelRadius);
                tireFRPos.y = static_cast<oni::common::real32>(carConfig.halfWidth / 2 + carConfig.wheelWidth);
                tireFRPos.z = carPosition.z;
                auto carTireFREntity = createTire(carEntityID, tireFRPos, tireSize);

                oni::math::vec3 tireFLPos;
                tireFLPos.x = static_cast<oni::common::real32>(carConfig.cgToFrontAxle - carConfig.wheelRadius);
                tireFLPos.y = static_cast<oni::common::real32>(-carConfig.halfWidth / 2 - carConfig.wheelWidth);
                tireFLPos.z = carPosition.z;
                auto carTireFLEntity = createTire(carEntityID, tireFLPos, tireSize);

                oni::math::vec3 tireRRPos;
                tireRRPos.x = static_cast<oni::common::real32>(-carConfig.cgToRearAxle);
                tireRRPos.y = static_cast<oni::common::real32>(carConfig.halfWidth / 2 + carConfig.wheelWidth);
                tireRRPos.z = carPosition.z;
                auto carTireRREntity = createTire(carEntityID, tireRRPos, tireSize);

                oni::math::vec3 tireRLPos;
                tireRLPos.x = static_cast<oni::common::real32>(-carConfig.cgToRearAxle);
                tireRLPos.y = static_cast<oni::common::real32>(-carConfig.halfWidth / 2 - carConfig.wheelWidth);
                tireRLPos.z = carPosition.z;
                auto carTireRLEntity = createTire(carEntityID, tireRLPos, tireSize);

                auto &car = mEntityManager->get<oni::component::Car>(carEntityID);
                car.tireFR = carTireFREntity;
                car.tireFL = carTireFLEntity;
                car.tireRR = carTireRREntity;
                car.tireRL = carTireRLEntity;

                car.gunEntity = carGunEntity;

                return carEntityID;
            }

            void ServerGame::removeCar(oni::common::EntityID carEntityID) {
                auto lock = mEntityManager->scopedLock();

                const auto &car = mEntityManager->get<oni::component::Car>(carEntityID);
                auto tireFL = car.tireFL;
                auto tireFR = car.tireFR;
                auto tireRL = car.tireRL;
                auto tireRR = car.tireRR;
                auto gunEntity = car.gunEntity;

                removeTire(carEntityID, tireFL);
                removeTire(carEntityID, tireFR);
                removeTire(carEntityID, tireRL);
                removeTire(carEntityID, tireRR);

                removeGun(carEntityID, gunEntity);

                oni::entities::removeShape(*mEntityManager, carEntityID);
                oni::entities::removePlacement(*mEntityManager, carEntityID);
                oni::entities::removeTexture(*mEntityManager, carEntityID);
                oni::entities::removePhysicalProperties(*mEntityManager, *mDynamics->getPhysicsWorld(), carEntityID);
                oni::entities::removeTag<oni::component::Tag_Dynamic>(*mEntityManager, carEntityID);
                oni::entities::removeCar(*mEntityManager, carEntityID);

                oni::entities::destroyEntity(*mEntityManager, carEntityID);
            }

            oni::common::EntityID ServerGame::createTire(oni::common::EntityID carEntityID,
                                                         const oni::math::vec3 &pos,
                                                         const oni::math::vec2 &size) {
                auto heading = static_cast<oni::common::real32>(oni::math::toRadians(90.0f));
                oni::math::vec3 scale{1.f, 1.f, 1.f};
                std::string texture = "resources/images/car/1/car-tire.png";

                auto entityID = createEntity(*mEntityManager);
                oni::entities::assignShapeLocal(*mEntityManager, entityID, size, pos.z);
                oni::entities::assignPlacement(*mEntityManager, entityID, pos, scale, heading);
                oni::entities::assignTextureToLoad(*mEntityManager, entityID, texture);
                oni::entities::assignTransformationHierarchy(*mEntityManager, carEntityID, entityID);
                oni::entities::assignTag<oni::component::Tag_Dynamic>(*mEntityManager, entityID);

                return entityID;
            }

            void ServerGame::removeTire(oni::common::EntityID carEntityID, oni::common::EntityID tireEntityID) {
                oni::entities::removeShape(*mEntityManager, tireEntityID);
                oni::entities::removePlacement(*mEntityManager, tireEntityID);
                oni::entities::removeTexture(*mEntityManager, tireEntityID);
                oni::entities::removeTransformationHierarchy(*mEntityManager, carEntityID, tireEntityID);
                oni::entities::removeTag<component::Tag_Dynamic>(*mEntityManager, tireEntityID);

                oni::entities::destroyEntity(*mEntityManager, tireEntityID);
            }

            EntityID ServerGame::createGun(EntityID carEntityID,
                                           const oni::math::vec3 &pos,
                                           const oni::math::vec2 &size) {
                oni::math::vec3 scale{1.f, 1.f, 1.f};
                std::string texture = "resources/images/minigun/1.png";

                auto entityID = createEntity(*mEntityManager);
                oni::entities::assignShapeLocal(*mEntityManager, entityID, size, pos.z);
                oni::entities::assignPlacement(*mEntityManager, entityID, pos, scale, 0.f);
                oni::entities::assignTextureToLoad(*mEntityManager, entityID, texture);
                oni::entities::assignTransformationHierarchy(*mEntityManager, carEntityID, entityID);
                oni::entities::assignTag<oni::component::Tag_Dynamic>(*mEntityManager, entityID);

                return entityID;
            }

            void ServerGame::removeGun(EntityID carEntityID, EntityID entityID) {
                oni::entities::removeShape(*mEntityManager, entityID);
                oni::entities::removePlacement(*mEntityManager, entityID);
                oni::entities::removeTexture(*mEntityManager, entityID);
                oni::entities::removeTransformationHierarchy(*mEntityManager, carEntityID, entityID);
                oni::entities::removeTag<component::Tag_Dynamic>(*mEntityManager, entityID);

                oni::entities::destroyEntity(*mEntityManager, entityID);
            }

            oni::common::EntityID ServerGame::createTruck() {
                auto vehicleZ = mZLayerManager->getZForEntity(oni::component::EntityType::VEHICLE);
                oni::math::vec2 size{1.0f, 3.0f};
                oni::math::vec3 worldPos{-60.0f, -30.0f, vehicleZ};
                oni::common::real32 heading = 0.f;
                std::string truckTexturePath = "resources/images/car/2/truck.png";
                oni::component::PhysicalProperties properties;
                properties.friction = 1.f;
                properties.density = 0.1f;
                properties.angularDamping = 2.f;
                properties.linearDamping = 2.f;
                properties.highPrecision = false;
                properties.bodyType = oni::component::BodyType::DYNAMIC;
                properties.physicalCategory = oni::component::PhysicalCategory::VEHICLE;

                auto lock = mEntityManager->scopedLock();
                auto entityID = oni::entities::createEntity(*mEntityManager);
                oni::entities::assignShapeLocal(*mEntityManager, entityID, size, vehicleZ);
                oni::entities::assignPlacement(*mEntityManager, entityID, worldPos, {1.f, 1.f, 0.f}, 0.f);
                oni::entities::assignPhysicalProperties(*mEntityManager, *mDynamics->getPhysicsWorld(), entityID,
                                                        worldPos,
                                                        size,
                                                        heading,
                                                        properties);
                oni::entities::assignTextureToLoad(*mEntityManager, entityID, truckTexturePath);
                oni::entities::assignTag<oni::component::Tag_Dynamic>(*mEntityManager, entityID);

                return entityID;
            }
        }
    }
}
