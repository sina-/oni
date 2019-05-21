#include <oni-server/game/oni-server-game.h>

#include <thread>

#include <oni-core/component/oni-component-visual.h>
#include <oni-core/common/oni-common-const.h>
#include <oni-core/gameplay/oni-gameplay-lap-tracker.h>
#include <oni-core/physics/oni-physics-dynamics.h>
#include <oni-core/network/oni-network-server.h>
#include <oni-core/entities/oni-entities-factory.h>
#include <oni-core/entities/oni-entities-client-data-manager.h>
#include <oni-core/entities/oni-entities-manager.h>
#include <oni-core/entities/oni-entities-entity.h>
#include <oni-core/entities/oni-entities-serialization.h>
#include <oni-core/math/oni-math-transformation.h>
// TODO: Compile fails if this is not here even though this file doesn't need it :( I had this issue in the past hmm...
#include <oni-core/math/oni-math-rand.h>
#include <oni-core/math/oni-math-z-layer-manager.h>


namespace oni {
    namespace server {
        namespace game {
            ServerGame::ServerGame(const network::Address &address) : Game(), mServerAddress(address) {
                mZLayerManager = std::make_unique<math::ZLayerManager>();
                mDynamics = std::make_unique<physics::Dynamics>(getTickFrequency());
                mEntityFactory = std::make_unique<oni::entities::EntityFactory>(entities::SimMode::SERVER,
                                                                                *mZLayerManager,
                                                                                *mDynamics->getPhysicsWorld());

                // TODO: Passing reference to unique_ptr and also exposing the b2World into the other classes!
                // Maybe I can expose subset of functionality I need from Dynamics class, maybe even better to call it
                // physics class part of which is dynamics.
                mTileWorld = std::make_unique<server::level::TileWorld>(*mEntityFactory,
                                                                        *mDynamics->getPhysicsWorld(),
                                                                        *mZLayerManager);

                mClientDataManager = std::make_unique<oni::entities::ClientDataManager>();
                mLapTracker = std::make_unique<gameplay::LapTracker>(mEntityFactory->getEntityManager(),
                                                                     *mZLayerManager);

                mServer = std::make_unique<network::Server>(&address, 16, 2);

                mServer->registerPacketHandler(network::PacketType::SETUP_SESSION,
                                               std::bind(&ServerGame::setupSessionPacketHandler, this,
                                                         std::placeholders::_1, std::placeholders::_2));
                mServer->registerPacketHandler(network::PacketType::CLIENT_INPUT,
                                               std::bind(&ServerGame::clientInputPacketHandler, this,
                                                         std::placeholders::_1,
                                                         std::placeholders::_2));
                mServer->registerPostDisconnectHook(std::bind(&ServerGame::postDisconnectHook, this,
                                                              std::placeholders::_1));

                mEntityFactory->getEntityManager().registerEventHandler<oni::game::Event_Collision, &network::Server::handleEvent_Collision>(
                        mServer.get());

                mEntityFactory->getEntityManager().registerEventHandler<oni::game::Event_SoundPlay, &network::Server::handleEvent_SoundPlay>(
                        mServer.get());

                mEntityFactory->getEntityManager().registerEventHandler<oni::game::Event_RocketLaunch, &network::Server::handleEvent_RocketLaunch>(
                        mServer.get());

                loadLevel();
            }

            ServerGame::~ServerGame() = default;

            void
            ServerGame::loadLevel() {
                mTruckEntity = spawnTruck();

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

            void
            ServerGame::setupSessionPacketHandler(const common::PeerID &clientID,
                                                  const std::string &data) {
                auto carEntity = spawnRaceCar();

                mServer->sendCarEntityID(carEntity, clientID);
                mServer->sendEntitiesAll(mEntityFactory->getEntityManager());

                mClientDataManager->addNewClient(clientID, carEntity);
            }

            void
            ServerGame::clientInputPacketHandler(const common::PeerID &clientID,
                                                 const std::string &data) {
                auto input = oni::entities::deserialize<io::Input>(data);
                // TODO: Avoid copy by using a unique_ptr or something
                mClientDataManager->setClientInput(clientID, input);
            }

            void
            ServerGame::postDisconnectHook(const common::PeerID &peerID) {
                auto clientCarEntityID = mClientDataManager->getEntityID(peerID);

                mEntityFactory->tagForRemoval(clientCarEntityID);
                mEntityFactory->flushEntityRemovals();
                mClientDataManager->deleteClient(peerID);
            }

            bool
            ServerGame::shouldTerminate() {
                return false;
            }

            void
            ServerGame::_poll() {
                mServer->poll();

                mServer->sendComponentsUpdate(mEntityFactory->getEntityManager());
                mServer->sendNewEntities(mEntityFactory->getEntityManager());

                {
                    auto &entityManager = mEntityFactory->getEntityManager();
                    mServer->broadcastDeletedEntities(entityManager);
                }
            }

            void
            ServerGame::_sim(const common::r64 tickTime) {
                // Fake lag
                //std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 4));

                {
                    mDynamics->tick(*mEntityFactory, mClientDataManager.get(), tickTime);
                }

                std::vector<component::WorldP2D> tickPositions{};
                {
                    // TODO: This is just awful :( so many locks. Accessing entities from registry without a view, which
                    // is slow and by the time the positions are passed to other systems, such as TileWorld, the entities
                    // might not even be there anymore. It won't crash because registry will be locked, but then what is the
                    // point of multi threading? Maybe I should drop this whole multi-thread everything shenanigans and just do
                    // things in sequence but have a pool of workers that can do parallel shit on demand for heavy lifting.
                    auto &manager = mEntityFactory->getEntityManager();
                    for (const auto &carEntity: mClientDataManager->getCarEntities()) {
                        const auto &pos = manager.get<component::WorldP3D>(carEntity);
                        tickPositions.push_back({pos.x, pos.y});
                    }
                }

                for (const auto &pos: tickPositions) {
                    mTileWorld->tick(pos);
                }

                mLapTracker->tick();
            }

            void
            ServerGame::_render(common::r64 simTime) {}

            void
            ServerGame::_display() {}

            void
            ServerGame::_finish() {
                mEntityFactory->getEntityManager().dispatchEvents();
                mServer->flush(); // TODO: Can this happen on separate thread?
            }

            void
            ServerGame::showFPS(common::i16 fps) {}

            void
            ServerGame::showSPS(common::i16 tps) {}

            void
            ServerGame::showPPS(common::i16 pps) {
                if (pps < 30) {
                    std::cout << "PPS " << pps << "\n";
                }
            }

            void
            ServerGame::showPT(common::i16) {}

            void
            ServerGame::showST(common::i16) {}

            void
            ServerGame::showRT(common::i16) {}

            common::EntityID
            ServerGame::spawnRaceCar() {
                auto vehicleZ = mZLayerManager->getZForEntity(oni::entities::EntityType::RACE_CAR);
                // TODO: All cars spawn in the same location!
                auto pos = component::WorldP3D{-70.f, -30.f, vehicleZ};
                math::vec2 size{2.5f, 1.1f};
                auto heading = component::Heading{0.f};
                std::string carTextureID = "resources/images/car/1/car.png";

                auto &manager = mEntityFactory->getEntityManager();

                auto carEntity = mEntityFactory->createEntity<oni::entities::EntityType::RACE_CAR>(
                        pos,
                        size,
                        heading,
                        carTextureID);

                mEntityFactory->tagForNetworkSync(carEntity);

                math::vec2 gunSize{2.f, 0.5f};
                auto gunPos = component::WorldP3D{0.5f, 0.f, mZLayerManager->getZForEntity(
                        oni::entities::EntityType::VEHICLE_GUN)};
                std::string gunTextureID = "resources/images/minigun/1.png";
                auto carGunEntity = mEntityFactory->createEntity<oni::entities::EntityType::VEHICLE_GUN>(
                        gunPos,
                        gunSize,
                        heading,
                        gunTextureID);
                mEntityFactory->tagForNetworkSync(carGunEntity);

                mEntityFactory->attach(carEntity, carGunEntity, oni::entities::EntityType::RACE_CAR,
                                       oni::entities::EntityType::VEHICLE_GUN);

                auto &carConfig = manager.get<component::CarConfig>(carEntity);
                std::string tireTextureID = "resources/images/car/1/car-tire.png";
                auto tireHeading = component::Heading{static_cast< common::r32>( math::toRadians(90.0f))};
                math::vec2 tireSize{
                        static_cast<common::r32>(carConfig.wheelWidth),
                        static_cast<common::r32>(carConfig.wheelRadius * 2.f)
                };

                std::array<math::vec2, 2> carTiresFront{
                        math::vec2{static_cast<common::r32>(carConfig.cgToFrontAxle - carConfig.wheelRadius),
                                   static_cast<common::r32>(carConfig.halfWidth / 2 + carConfig.wheelWidth)},
                        math::vec2{static_cast<common::r32>(carConfig.cgToFrontAxle - carConfig.wheelRadius),
                                   static_cast<common::r32>(-carConfig.halfWidth / 2 - carConfig.wheelWidth)}
                };
                std::array<math::vec2, 2> carTiresRear{
                        math::vec2{static_cast<common::r32>(-carConfig.cgToRearAxle),
                                   static_cast<common::r32>(carConfig.halfWidth / 2 + carConfig.wheelWidth)},
                        math::vec2{static_cast<common::r32>(-carConfig.cgToRearAxle),
                                   static_cast<common::r32>(-carConfig.halfWidth / 2 - carConfig.wheelWidth)}
                };

                for (auto &&carTire: carTiresFront) {
                    auto tirePos = component::WorldP3D{carTire.x, carTire.y, vehicleZ};
                    auto tireEntity = mEntityFactory->createEntity<oni::entities::EntityType::VEHICLE_TIRE_FRONT>(
                            tirePos,
                            tireSize,
                            tireHeading,
                            tireTextureID);
                    mEntityFactory->tagForNetworkSync(tireEntity);

                    mEntityFactory->attach(carEntity, tireEntity, oni::entities::EntityType::RACE_CAR,
                                           oni::entities::EntityType::VEHICLE_TIRE_FRONT);
                }

                for (auto &&carTire: carTiresRear) {
                    auto tirePos = component::WorldP3D{carTire.x, carTire.y, vehicleZ};
                    auto tireEntity = mEntityFactory->createEntity<oni::entities::EntityType::VEHICLE_TIRE_REAR>(
                            tirePos,
                            tireSize,
                            tireHeading,
                            tireTextureID);
                    mEntityFactory->tagForNetworkSync(tireEntity);

                    mEntityFactory->attach(carEntity, tireEntity, oni::entities::EntityType::RACE_CAR,
                                           oni::entities::EntityType::VEHICLE_TIRE_REAR);
                }

                return carEntity;
            }

            common::EntityID
            ServerGame::spawnTruck() {
                auto vehicleZ = mZLayerManager->getZForEntity(oni::entities::EntityType::VEHICLE);
                math::vec2 size{4.0f, 12.0f};
                auto worldPos = component::WorldP3D{-20.0f, -30.0f, vehicleZ};
                auto heading = component::Heading{0.f};
                std::string textureID = "resources/images/car/2/truck.png";

                auto entityID = mEntityFactory->createEntity<oni::entities::EntityType::VEHICLE>(
                        worldPos, size,
                        heading,
                        textureID);
                mEntityFactory->tagForNetworkSync(entityID);
                return entityID;
            }
        }
    }
}
