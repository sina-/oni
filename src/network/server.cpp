#include <oni-core/network/server.h>

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <cassert>

#include <Box2D/Box2D.h>
#include <enet/enet.h>
#include <entt/entity/registry.hpp>
#include <cereal/archives/portable_binary.hpp>

#include <oni-core/network/packet.h>
#include <oni-core/entities/serialization.h>
#include <oni-core/components/geometry.h>
#include <oni-core/entities/manage-hierarchy.h>
#include <oni-core/physics/dynamics.h>

// TODO: This shouldn't be included here!
#include <oni-core/graphics/debug-draw-box2d.h>


namespace oni {
    namespace network {
        Server::~Server() = default;

        Server::Server(const Address *address, common::uint8 numClients, common::uint8 numChannels,
                       entt::DefaultRegistry &foregroundEntities, physics::Dynamics &dynamics) :
                Peer::Peer(address, numClients, numChannels, 0, 0),
                mForegroundEntities{foregroundEntities}, mDynamics{dynamics} {
        }

        std::vector<entities::entityID> Server::getCarEntities() const {
            std::vector<entities::entityID> carEntities{};
            for (auto entity: mClientToCarEntity) {
                carEntities.push_back(entity.second);
            }
            return carEntities;
        }

        void Server::postConnectHook(const ENetEvent *event) {
            auto carEntity = entities::createVehicleEntity(mForegroundEntities, *mDynamics.getPhysicsWorld());

            auto carTexture = components::Texture{};
            std::string carTexturePath = "resources/images/car/1/car.png";
            carTexture.filePath = carTexturePath;
            carTexture.status = components::TextureStatus::NEEDS_LOADING_USING_PATH;
            entities::assignTexture(mForegroundEntities, carEntity, carTexture);

            // IMPORTANT NOTE: Newbie trap! carConfig must be a copy, otherwise createEntity calls will resize the data
            // storage and the old reference will be invalidated and then we end-up getting garbage :(
            auto carConfig = mForegroundEntities.get<components::CarConfig>(carEntity);

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
            auto carTireFREntity = entities::createDynamicEntity(mForegroundEntities, tireSize, tireFRPos,
                                                                 tireRotation, math::vec3{1.0f, 1.0f, 0.0f});
            entities::TransformationHierarchy::createTransformationHierarchy(mForegroundEntities, carEntity,
                                                                             carTireFREntity);
            entities::assignTexture(mForegroundEntities, carTireFREntity, carTireTexture);

            auto tireFLPos = math::vec3{};
            tireFLPos.x = static_cast<common::real32>(carConfig.cgToFrontAxle - carConfig.wheelRadius);
            tireFLPos.y = static_cast<common::real32>(-carConfig.halfWidth / 2 - carConfig.wheelWidth);
            auto carTireFLEntity = entities::createDynamicEntity(mForegroundEntities, tireSize, tireFLPos,
                                                                 tireRotation, math::vec3{1.0f, 1.0f, 0.0f});
            entities::TransformationHierarchy::createTransformationHierarchy(mForegroundEntities, carEntity,
                                                                             carTireFLEntity);
            entities::assignTexture(mForegroundEntities, carTireFLEntity, carTireTexture);

            auto tireRRPos = math::vec3{};
            tireRRPos.x = static_cast<common::real32>(-carConfig.cgToRearAxle);
            tireRRPos.y = static_cast<common::real32>(carConfig.halfWidth / 2 + carConfig.wheelWidth);
            auto carTireRREntity = entities::createDynamicEntity(mForegroundEntities, tireSize, tireRRPos,
                                                                 tireRotation, math::vec3{1.0f, 1.0f, 0.0f});
            entities::TransformationHierarchy::createTransformationHierarchy(mForegroundEntities, carEntity,
                                                                             carTireRREntity);
            entities::assignTexture(mForegroundEntities, carTireRREntity, carTireTexture);

            auto tireRLPos = math::vec3{};
            tireRLPos.x = static_cast<common::real32>(-carConfig.cgToRearAxle);
            tireRLPos.y = static_cast<common::real32>(-carConfig.halfWidth / 2 - carConfig.wheelWidth);
            auto carTireRLEntity = entities::createDynamicEntity(mForegroundEntities, tireSize, tireRLPos,
                                                                 tireRotation, math::vec3{1.0f, 1.0f, 0.0f});
            entities::TransformationHierarchy::createTransformationHierarchy(mForegroundEntities, carEntity,
                                                                             carTireRLEntity);
            entities::assignTexture(mForegroundEntities, carTireRLEntity, carTireTexture);

            auto &car = mForegroundEntities.get<components::Car>(carEntity);
            car.tireFR = carTireFREntity;
            car.tireFL = carTireFLEntity;
            car.tireRR = carTireRREntity;
            car.tireRL = carTireRLEntity;

            mClientToCarEntity[event->peer->connectID] = carEntity;

            auto packet = EntityPacket{carEntity};
            auto data = serialize<EntityPacket>(packet);
            auto type = PacketType::CAR_ENTITY_ID;
            send(type, std::move(data), event->peer);
        }

        void Server::postDisconnectHook(const ENetEvent *event) {

        }

        void tick(entt::DefaultRegistry &registry) {

        }

        void Server::handle(ENetPeer *peer, enet_uint8 *data, size_t size, PacketType header) {
            switch (header) {
                case (PacketType::PING): {
                    auto packet = deserialize<PingPacket>(data, size);
                    std::cout << packet.timestamp << std::endl;

                    // TODO: Can't just send the packet like this, its missing the header and the size is only
                    // the size of payload
                    //send(data, size, peer);

                    break;
                }
                case (PacketType::MESSAGE): {
                    auto packet = deserialize<DataPacket>(data, size);
                    std::cout << packet.data << std::endl;
                    break;
                }
                case (PacketType::ENTITY): {
                    break;
                }
                default: {
                    // TODO: Need to keep stats on clients with bad packets and block them when threshold reaches.
                    std::cout << "Unknown packet!" << std::endl;
                    break;
                }
            }
        }

        void Server::sendWorldData(entt::DefaultRegistry &registry) {
            auto data = entities::serialize(registry);
            auto type = PacketType::WORLD_DATA;

            broadcast(type, std::move(data));
        }
    }
}
