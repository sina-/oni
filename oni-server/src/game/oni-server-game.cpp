#include <oni-server/game/oni-server-game.h>

#include <thread>

#include <oni-core/component/oni-component-visual.h>
#include <oni-core/common/oni-common-const.h>
#include <oni-core/entities/oni-entities-client-data-manager.h>
#include <oni-core/entities/oni-entities-manager.h>
#include <oni-core/entities/oni-entities-entity.h>
#include <oni-core/entities/oni-entities-serialization.h>
#include <oni-core/gameplay/oni-gameplay-lap-tracker.h>
#include <oni-core/math/oni-math-transformation.h>
#include <oni-core/math/oni-math-z-layer-manager.h>
#include <oni-core/network/oni-network-server.h>
#include <oni-core/physics/oni-physics-dynamics.h>

#include <oni-server/level/oni-server-level-tile-world.h>


namespace oni {
    namespace server {
        namespace game {
            ServerGame::ServerGame(const network::Address &address) : Game(), mServerAddress(address) {
                mZLayerManager = std::make_unique<math::ZLayerManager>();
                mDynamics = std::make_unique<physics::Dynamics>();
                mEntityManager = std::make_unique<oni::entities::EntityManager>(entities::SimMode::SERVER,
                                                                                *mZLayerManager,
                                                                                *mDynamics->getPhysicsWorld());

                // TODO: Passing reference to unique_ptr and also exposing the b2World into the other classes!
                // Maybe I can expose subset of functionality I need from Dynamics class, maybe even better to call it
                // physics class part of which is dynamics.
                mTileWorld = std::make_unique<server::level::TileWorld>(*mEntityManager,
                                                                        *mDynamics->getPhysicsWorld(),
                                                                        *mZLayerManager);

                mClientDataManager = std::make_unique<oni::entities::ClientDataManager>();
                mLapTracker = std::make_unique<gameplay::LapTracker>(*mEntityManager,
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

                mEntityManager->registerEventHandler<oni::game::Event_Collision, &network::Server::handleEvent_Collision>(
                        mServer.get());

                mEntityManager->registerEventHandler<oni::game::Event_SoundPlay, &network::Server::handleEvent_SoundPlay>(
                        mServer.get());

                mEntityManager->registerEventHandler<oni::game::Event_RocketLaunch, &network::Server::handleEvent_RocketLaunch>(
                        mServer.get());

                loadLevel();
            }

            ServerGame::~ServerGame() = default;

            void
            ServerGame::loadLevel() {
                spawnTruck();

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
                mServer->sendEntitiesAll(*mEntityManager);

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

                mEntityManager->markForDeletion(clientCarEntityID);
                mEntityManager->flushDeletions();
                mClientDataManager->deleteClient(peerID);
            }

            bool
            ServerGame::shouldTerminate() {
                return false;
            }

            void
            ServerGame::_poll() {
                mServer->poll();

                mServer->sendComponentsUpdate(*mEntityManager);
                mServer->sendNewEntities(*mEntityManager);

                mServer->broadcastDeletedEntities(*mEntityManager);
            }

            void
            ServerGame::_sim(const common::r64 tickTime) {
                // Fake lag
                //std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 4));

                {
                    mDynamics->tick(*mEntityManager, mClientDataManager.get(), tickTime);
                }

                std::vector<component::WorldP2D> tickPositions{};
                {
                    // TODO: This is just awful :( so many locks. Accessing entities from registry without a view, which
                    // is slow and by the time the positions are passed to other systems, such as TileWorld, the entities
                    // might not even be there anymore. It won't crash because registry will be locked, but then what is the
                    // point of multi threading? Maybe I should drop this whole multi-thread everything shenanigans and just do
                    // things in sequence but have a pool of workers that can do parallel shit on demand for heavy lifting.
                    for (const auto &carEntity: mClientDataManager->getCarEntities()) {
                        const auto &pos = mEntityManager->get<component::WorldP3D>(carEntity);
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
                mEntityManager->dispatchEvents();
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
                auto pos = component::WorldP3D{-0, -0, vehicleZ};
                auto size = math::vec2{2.5f, 1.1f};

                auto carEntity = mEntityManager->createEntity_RaceCar();
                mEntityManager->setWorldP3D(carEntity, pos.x, pos.y, pos.z);
                mEntityManager->setScale(carEntity, size.x, size.y);
                mEntityManager->createPhysics(carEntity, pos, size, 0);

                auto carGunEntity = mEntityManager->createEntity_VehicleGun();
                mEntityManager->setWorldP3D(carGunEntity, 0.5f, 0.f, mZLayerManager->getZForEntity(
                        oni::entities::EntityType::VEHICLE_GUN));
                mEntityManager->setScale(carGunEntity, 2.f, 0.5f);

                mEntityManager->attach(carEntity, carGunEntity, oni::entities::EntityType::RACE_CAR,
                                       oni::entities::EntityType::VEHICLE_GUN);

                auto &carConfig = mEntityManager->get<component::CarConfig>(carEntity);
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
                    auto tireEntity = mEntityManager->createEntity_VehicleTireFront();
                    mEntityManager->setWorldP3D(tireEntity, carTire.x, carTire.y, vehicleZ);
                    mEntityManager->setScale(tireEntity, tireSize.x, tireSize.y);
                    mEntityManager->setHeading(tireEntity, tireHeading.value);

                    mEntityManager->attach(carEntity, tireEntity, oni::entities::EntityType::RACE_CAR,
                                           oni::entities::EntityType::VEHICLE_TIRE_FRONT);
                }

                for (auto &&carTire: carTiresRear) {
                    auto tireEntity = mEntityManager->createEntity_VehicleTireRear();
                    mEntityManager->setWorldP3D(tireEntity, carTire.x, carTire.y, vehicleZ);
                    mEntityManager->setScale(tireEntity, tireSize.x, tireSize.y);
                    mEntityManager->setHeading(tireEntity, tireHeading.value);

                    mEntityManager->attach(carEntity, tireEntity, oni::entities::EntityType::RACE_CAR,
                                           oni::entities::EntityType::VEHICLE_TIRE_REAR);
                }

                return carEntity;
            }

            common::EntityID
            ServerGame::spawnTruck() {
                auto vehicleZ = mZLayerManager->getZForEntity(oni::entities::EntityType::VEHICLE);
                auto size = math::vec2{4.0f, 12.0f};
                auto pos = component::WorldP3D{-20.0f, -30.0f, vehicleZ};
                auto heading = 0.f;

                auto id = mEntityManager->createEntity_Vehicle();

                mEntityManager->setWorldP3D(id, pos.x, pos.y, pos.z);
                mEntityManager->setScale(id, size.x, size.y);
                mEntityManager->setHeading(id, heading);
                mEntityManager->createPhysics(id, pos, size, heading);

                return id;
            }
        }
    }
}
