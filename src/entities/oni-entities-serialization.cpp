#include <oni-core/entities/oni-entities-serialization.h>

#include <sstream>

#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/map.hpp>
#include <cereal/archives/portable_binary.hpp>

#include <oni-core/entities/oni-entities-manager.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-hierarchy.h>
#include <oni-core/component/oni-component-visual.h>
#include <oni-core/component/oni-component-gameplay.h>
#include <oni-core/component/oni-component-physic.h>
#include <oni-core/gameplay/oni-gameplay-lap-tracker.h>


namespace oni {
    namespace gameplay {
        template<class Archive>
        void
        serialize(Archive &archive,
                  gameplay::CarLapInfo &data) {
            archive(data.entityID, data.lap, data.lapTimeS, data.bestLapTimeS);
        }
    }

    namespace component {
        template<class Archive>
        void
        serialize(Archive &archive,
                  component::Shape &data) {
            archive(data.vertexA, data.vertexB, data.vertexC, data.vertexD);
        }

        template<class Archive>
        void
        serialize(Archive &archive,
                  component::Point &data) {
            archive(data);
        }

        template<class Archive>
        void
        serialize(Archive &archive,
                  component::SmokeEmitterCD &data) {
            archive(data.currentCD);
        }

        template<class Archive>
        void
        serialize(Archive &archive,
                  component::WorldP2D &data) {
            archive(data.value);
        }

        template<class Archive>
        void
        serialize(Archive &archive,
                  component::Size &data) {
            archive(data.value);
        }

        template<class Archive>
        void
        serialize(Archive &archive,
                  component::Trail &data) {
            archive(data.previousPos, data.velocity);
        }

        template<class Archive>
        void
        serialize(Archive &archive,
                  component::EntityAttachment &data) {
            archive(data.entities, data.entityTypes);
        }

        template<class Archive>
        void
        serialize(Archive &archive,
                  component::EntityAttachee &data) {
            archive(data.entityID, data.entityType);
        }

        template<class Archive>
        void
        serialize(Archive &archive,
                  component::Heading &data) {
            archive(data.value);
        }

        template<class Archive>
        void
        serialize(Archive &archive,
                  component::Scale &data) {
            archive(data.value);
        }

        template<class Archive>
        void
        serialize(Archive &archive,
                  component::CarConfig &data) {
            archive(data.gravity,
                    data.mass,
                    data.inertialScale,
                    data.halfWidth,
                    data.cgToFront,
                    data.cgToRear,
                    data.cgToFrontAxle,
                    data.cgToRearAxle,
                    data.cgHeight,
                    data.wheelRadius,
                    data.wheelWidth,
                    data.tireGrip,
                    data.lockGrip,
                    data.engineForce,
                    data.brakeForce,
                    data.eBrakeForce,
                    data.weightTransfer,
                    data.maxSteer,
                    data.cornerStiffnessFront,
                    data.cornerStiffnessRear,
                    data.airResist,
                    data.rollResist,
                    data.gearRatio,
                    data.differentialRatio
            );
        }

        template<class Archive>
        void
        serialize(Archive &archive,
                  component::Car &data) {
            archive(
                    data.heading,
                    data.velocityAbsolute,
                    data.angularVelocity,
                    data.steer,
                    data.steerAngle,
                    data.inertia,
                    data.wheelBase,
                    data.axleWeightRatioFront,
                    data.axleWeightRatioRear,
                    data.rpm,
                    data.maxVelocityAbsolute,
                    data.accumulatedEBrake,
                    data.slipAngleFront,
                    data.slipAngleRear,
                    data.position,
                    data.velocity,
                    data.velocityLocal,
                    data.acceleration,
                    data.accelerationLocal,
                    data.accelerating,
                    data.slippingFront,
                    data.slippingRear,
                    data.smoothSteer,
                    data.safeSteer,
                    data.distanceFromCamera,
                    data.isColliding
            );
        }

        template<class Archive>
        void
        serialize(Archive &archive,
                  component::TransformParent &data) {
            archive(data.parent, data.transform);
        }

        template<class Archive>
        void
        serialize(Archive &archive,
                  component::Tag_Static &) {}

        template<class Archive>
        void
        serialize(Archive &archive,
                  component::Tag_Dynamic &) {}

        template<class Archive>
        void
        serialize(Archive &archive,
                  component::Tag_TextureShaded &) {}

        template<class Archive>
        void
        serialize(Archive &archive,
                  component::Tag_ColorShaded &) {}

        template<class Archive>
        void
        serialize(Archive &archive,
                  component::Tag_Audible &) {}

        template<class Archive>
        void
        serialize(Archive &archive,
                  component::Appearance &data) {
            archive(data.color);
        }

        template<class Archive>
        void
        serialize(Archive &archive,
                  component::Image &data) {
            archive(data.data, data.width, data.height);
        }

        template<class Archive>
        void
        serialize(Archive &archive,
                  component::Age &data) {
            archive(data.currentAge, data.maxAge);
        }

        template<class Archive>
        void
        serialize(Archive &archive,
                  component::Texture &data) {
            archive(data.image, data.textureID, data.format, data.type, data.path,
                    data.uv, data.status);
        }
    }

    namespace entities {
        std::string
        serialize(entities::EntityManager &manager,
                  entities::SnapshotType snapshotType) {
            std::stringstream storage{};
            {
                cereal::PortableBinaryOutputArchive output{storage};
                manager.snapshot<
                        cereal::PortableBinaryOutputArchive,
                        component::Car,
                        component::CarConfig,
                        component::WorldP3D,
                        component::WorldP2D,
                        component::Heading,
                        component::Scale,
                        //component::SmokeEmitterCD,
                        component::Shape,
                        component::Size,
                        component::Point,
                        component::Appearance,
                        component::Age,
                        component::Texture,
                        component::Trail,
                        component::EntityAttachment,
                        component::EntityAttachee,
                        component::TransformParent,
                        component::SoundTag,

                        entities::EntityType,
                        gameplay::CarLapInfo,

                        // TODO: This is a cluster fuck of a design. This is just a raw pointer. Client doesnt need
                        // to know what it points to at the moment because sever does the physics calculations and only
                        // send the results back to the client, so I can skip it. But for the future I have to
                        // find a solution to this shit.
                        //components::PhysicalProperties,

                        component::Tag_Dynamic,
                        component::Tag_Static,
                        component::Tag_TextureShaded,
                        component::Tag_ColorShaded,
                        component::Tag_Audible
                                >(output, snapshotType);
            }

            return storage.str();
        }

        void
        deserialize(EntityManager &manager,
                    const std::string &data,
                    entities::SnapshotType snapshotType) {
            std::stringstream storage;
            storage.str(data);

            {
                cereal::PortableBinaryInputArchive input{storage};
                manager.restore<
                        cereal::PortableBinaryInputArchive,
                        component::Car,
                        component::CarConfig,
                        component::WorldP3D,
                        component::WorldP2D,
                        component::Heading,
                        component::Scale,
                        //component::SmokeEmitterCD,
                        component::Shape,
                        component::Size,
                        component::Point,
                        component::Appearance,
                        component::Age,
                        component::Texture,
                        component::Trail,
                        component::EntityAttachment,
                        component::EntityAttachee,
                        component::TransformParent,
                        component::SoundTag,

                        entities::EntityType,
                        gameplay::CarLapInfo,

                        //components::PhysicalProperties,

                        component::Tag_Dynamic,
                        component::Tag_Static,
                        component::Tag_TextureShaded,
                        component::Tag_ColorShaded,
                        component::Tag_Audible
                               >(snapshotType, input,
                        // NOTE: Entities might keep references to other entities but those ids might change during
                        // client-server sync process, this will make sure that the client side does the correct
                        // mapping from client side ids to server side ids for each entity.
                                 &component::EntityAttachment::entities,
                                 &component::EntityAttachee::entityID,
                                 &gameplay::CarLapInfo::entityID
                );
            }
        }
    }
}