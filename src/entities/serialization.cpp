#include <oni-core/entities/serialization.h>

#include <sstream>

#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/portable_binary.hpp>

#include <oni-core/entities/entity-manager.h>
#include <oni-core/components/geometry.h>
#include <oni-core/components/hierarchy.h>
#include <oni-core/components/visual.h>
#include <oni-core/components/world-data-status.h>
#include <oni-core/components/gameplay.h>


namespace oni {
    namespace components {
        template<class Archive>
        void serialize(Archive &archive, components::CarLapInfo &carLapInfo) {
            archive(carLapInfo.entityID, carLapInfo.lap, carLapInfo.lapTimeS, carLapInfo.bestLapTimeS);
        }

        template<class Archive>
        void serialize(Archive &archive, components::Shape &shape) {
            archive(shape.vertexA, shape.vertexB, shape.vertexC, shape.vertexD);
        }

        template<class Archive>
        void serialize(Archive &archive, components::Placement &placement) {
            archive(placement.position, placement.rotation, placement.scale);
        }

        template<class Archive>
        void serialize(Archive &archive, components::CarConfig &carConfig) {
            archive(carConfig.gravity,
                    carConfig.mass,
                    carConfig.inertialScale,
                    carConfig.halfWidth,
                    carConfig.cgToFront,
                    carConfig.cgToRear,
                    carConfig.cgToFrontAxle,
                    carConfig.cgToRearAxle,
                    carConfig.cgHeight,
                    carConfig.wheelRadius,
                    carConfig.wheelWidth,
                    carConfig.tireGrip,
                    carConfig.lockGrip,
                    carConfig.engineForce,
                    carConfig.brakeForce,
                    carConfig.eBrakeForce,
                    carConfig.weightTransfer,
                    carConfig.maxSteer,
                    carConfig.cornerStiffnessFront,
                    carConfig.cornerStiffnessRear,
                    carConfig.airResist,
                    carConfig.rollResist,
                    carConfig.gearRatio,
                    carConfig.differentialRatio
            );
        }

        template<class Archive>
        void serialize(Archive &archive, components::Car &car) {
            archive(
                    car.heading,
                    car.velocityAbsolute,
                    car.angularVelocity,
                    car.steer,
                    car.steerAngle,
                    car.inertia,
                    car.wheelBase,
                    car.axleWeightRatioFront,
                    car.axleWeightRatioRear,
                    car.rpm,
                    car.maxVelocityAbsolute,
                    car.accumulatedEBrake,
                    car.slipAngleFront,
                    car.slipAngleRear,
                    car.position,
                    car.velocity,
                    car.velocityLocal,
                    car.acceleration,
                    car.accelerationLocal,
                    car.accelerating,
                    car.slippingFront,
                    car.slippingRear,
                    car.smoothSteer,
                    car.safeSteer,
                    car.distanceFromCamera,

                    car.tireFR,
                    car.tireFL,
                    car.tireRR,
                    car.tireRL,

                    car.isColliding
            );
        }

        template<class Archive>
        void serialize(Archive &archive, components::TransformParent &transformParent) {
            archive(transformParent.parent, transformParent.transform);
        }

        template<class Archive>
        void serialize(Archive &archive, components::Tag_Static &) {}

        template<class Archive>
        void serialize(Archive &archive, components::Tag_Dynamic &) {}

        template<class Archive>
        void serialize(Archive &archive, components::Tag_Vehicle &) {}

        template<class Archive>
        void serialize(Archive &archive, components::Tag_TextureShaded &) {}

        template<class Archive>
        void serialize(Archive &archive, components::Tag_ColorShaded &) {}

        template<class Archive>
        void serialize(Archive &archive, components::Appearance &appearance) {
            archive(appearance.color);
        }

        template<class Archive>
        void serialize(Archive &archive, components::Texture &texture) {
            archive(texture.width, texture.height, texture.textureID, texture.format, texture.type, texture.filePath,
                    texture.uv, texture.data, texture.status);
        }
    }

    namespace math {
        template<class Archive>
        void serialize(Archive &archive, math::mat4 &mat4) {
            archive(mat4.columns);
        }

        template<class Archive>
        void serialize(Archive &archive, math::vec2 &vec2) {
            archive(vec2.x, vec2.y);
        }

        template<class Archive>
        void serialize(Archive &archive, math::vec3 &vec3) {
            archive(vec3.x, vec3.y, vec3.z);
        }

        template<class Archive>
        void serialize(Archive &archive, math::vec4 &vec4) {
            archive(vec4.x, vec4.y, vec4.z, vec4.w);
        }
    }

    namespace entities {

        std::string serialize(entities::EntityManager &manager, components::WorldDataStatus lifeTime) {
            std::stringstream storage{};
            {
                auto lock = manager.scopedLock();
                cereal::PortableBinaryOutputArchive output{storage};
                manager.snapshot<cereal::PortableBinaryOutputArchive,
                        components::Car,
                        components::CarConfig,
                        components::Placement,
                        components::CarLapInfo,
                        //components::Chunk,
                        components::Shape,
                        components::Appearance,
                        components::Texture,
                        // TODO: This is a cluster fuck of a design. This is just a raw pointer. Client doesnt need
                        // to know what it points to at the moment because sever does the physics calculations and only
                        // send the results back to the client, so I can skip it. But for the future I have to
                        // find a solution to this shit.
                        //components::PhysicalProperties,
                        components::TransformParent,
                        components::Tag_Dynamic,
                        components::Tag_Vehicle,
                        components::Tag_TextureShaded,
                        components::Tag_ColorShaded,
                        components::Tag_Static
                >(output, lifeTime);
            }

            return storage.str();
        }

        void deserialize(EntityManager &manager, const std::string &data, components::WorldDataStatus lifeTime) {
            std::stringstream storage;
            storage.str(data);

            {
                auto lock = manager.scopedLock();
                cereal::PortableBinaryInputArchive input{storage};
                manager.restore<cereal::PortableBinaryInputArchive,
                        components::Car,
                        components::CarConfig,
                        components::Placement,
                        components::CarLapInfo,
                        //components::Chunk,
                        components::Shape,
                        components::Appearance,
                        components::Texture,
                        //components::PhysicalProperties,
                        components::TransformParent,
                        components::Tag_Dynamic,
                        components::Tag_Vehicle,
                        components::Tag_TextureShaded,
                        components::Tag_ColorShaded,
                        components::Tag_Static
                >(lifeTime, input,
                        // NOTE: Car entity keeps a reference to tire entities but those ids might change during
                        // client-server sync process, this will make sure that the client side does the correct
                        // mapping from client side tire ids to server side ids for each Car.
                  &components::Car::tireFR,
                  &components::Car::tireFL,
                  &components::Car::tireRR,
                  &components::Car::tireRL,
                  &components::CarLapInfo::entityID
                );
            }
        }
    }
}