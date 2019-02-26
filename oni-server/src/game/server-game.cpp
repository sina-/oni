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
#include <oni-core/math/transformation.h>

#include <oni-server/entities/tile-world.h>


namespace oni {
    namespace server {
        namespace game {
            ServerGame::ServerGame(const oni::network::Address &address) : Game(), mServerAddress(address) {
                mZLayerManager = std::make_unique<oni::math::ZLayerManager>();
                mEntityManager = std::make_unique<oni::entities::EntityManager>();
                mDynamics = std::make_unique<oni::physics::Dynamics>(getTickFrequency());
                mEntityFactory = std::make_unique<oni::entities::EntityFactory>(*mEntityManager, *mZLayerManager,
                                                                                *mDynamics->getPhysicsWorld());

                // TODO: Passing reference to unique_ptr and also exposing the b2World into the other classes!
                // Maybe I can expose subset of functionalities I need from Dynamics class, maybe even better to call it
                // physics class part of which is dynamics.
                mTileWorld = std::make_unique<oni::server::entities::TileWorld>(*mEntityManager,
                                                                                *mEntityFactory,
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
                mServer->registerPostDisconnectHook(std::bind(&ServerGame::postDisconnectHook, this,
                                                              std::placeholders::_1));

                loadLevel();
            }

            ServerGame::~ServerGame() = default;

            void ServerGame::loadLevel() {
                mTruckEntity = createTruck();

                mTileWorld->genDemoRaceCourse();

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
                auto carEntity = spawnRaceCar();

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

            void ServerGame::postDisconnectHook(const oni::common::PeerID &peerID) {
                auto clientDataLock = mClientDataManager->scopedLock();
                auto clientCarEntityID = mClientDataManager->getEntityID(peerID);

                removeRaceCar(clientCarEntityID);
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
                if (pps < 30) {
                    std::cout << "PPS " << pps << "\n";
                }
            }

            void ServerGame::showPET(oni::common::int16 pet) {}

            void ServerGame::showSET(oni::common::int16 set) {}

            void ServerGame::showRET(oni::common::int16 ret) {}

            common::EntityID ServerGame::spawnRaceCar() {
                auto vehicleZ = mZLayerManager->getZForEntity(component::EntityType::RACE_CAR);
                // TODO: All cars spawn in the same location!
                math::vec3 pos{-70.f, -30.f, vehicleZ};
                math::vec2 size{2.5f, 1.1f};
                common::real32 heading = 0.f;
                std::string carTextureID = "resources/images/car/1/car.png";

                auto lock = mEntityFactory->scopedLock();

                auto carEntity = mEntityFactory->createEntity
                        <component::EntityType::RACE_CAR>(pos,
                                                          size,
                                                          heading,
                                                          carTextureID);

                oni::math::vec2 gunSize{2.f, 0.5f};
                oni::math::vec3 gunPos{1.f, 0.f, mZLayerManager->getZForEntity(component::EntityType::VEHICLE_GUN)};
                std::string gunTextureID = "resources/images/minigun/1.png";
                auto carGunEntity = mEntityFactory->createEntity
                        <component::EntityType::VEHICLE_GUN>(gunPos,
                                                             gunSize,
                                                             heading,
                                                             gunTextureID);

                oni::entities::attach(*mEntityManager, carEntity, carGunEntity, oni::component::EntityType::RACE_CAR,
                                      oni::component::EntityType::VEHICLE_GUN);

                auto &carConfig = mEntityManager->get<component::CarConfig>(carEntity);
                std::string tireTextureID = "resources/images/car/1/car-tire.png";
                auto tireRotation = static_cast<oni::common::real32>(oni::math::toRadians(90.0f));
                math::vec2 tireSize{
                        static_cast<common::real32>(carConfig.wheelWidth),
                        static_cast<common::real32>(carConfig.wheelRadius * 2)
                };

                std::array<math::vec2, 4> carTires{
                        math::vec2{static_cast<common::real32>(carConfig.cgToFrontAxle - carConfig.wheelRadius),
                                   static_cast<common::real32>(carConfig.halfWidth / 2 + carConfig.wheelWidth)},
                        math::vec2{static_cast<common::real32>(carConfig.cgToFrontAxle - carConfig.wheelRadius),
                                   static_cast<common::real32>(-carConfig.halfWidth / 2 - carConfig.wheelWidth)},
                        math::vec2{static_cast<common::real32>(-carConfig.cgToRearAxle),
                                   static_cast<common::real32>(carConfig.halfWidth / 2 + carConfig.wheelWidth)},
                        math::vec2{static_cast<common::real32>(-carConfig.cgToRearAxle),
                                   static_cast<common::real32>(-carConfig.halfWidth / 2 - carConfig.wheelWidth)},
                };

                for (auto &&carTire: carTires) {
                    math::vec3 tirePos{carTire.x, carTire.y, vehicleZ};
                    auto tireEntity = mEntityFactory->createEntity<oni::component::EntityType::VEHICLE_TIRE>(
                            tirePos,
                            tireSize,
                            tireRotation,
                            tireTextureID);
                    oni::entities::attach(*mEntityManager, carEntity, tireEntity, oni::component::EntityType::RACE_CAR,
                                          oni::component::EntityType::VEHICLE_TIRE);
                }

                return carEntity;
            }

            void ServerGame::removeRaceCar(oni::common::EntityID carEntityID) {
                auto lock = mEntityFactory->scopedLock();
                mEntityFactory->removeEntity<oni::component::EntityType::RACE_CAR>(carEntityID);
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

                auto lock = mEntityFactory->scopedLock();
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
