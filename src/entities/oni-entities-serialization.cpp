#include <oni-core/entities/oni-entities-serialization.h>

#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/deque.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/map.hpp>
#include <cereal/archives/portable_binary.hpp>

#include <oni-core/entities/oni-entities-manager.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-visual.h>
#include <oni-core/component/oni-component-physics.h>


namespace oni {
    template<class Archive>
    void
    serialize(Archive &archive,
              Rectangle &data) {
        archive(data.A, data.B, data.C, data.D);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              Point &data) {
        archive(data);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              CoolDown &data) {
        archive(data.current, data.initial);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              WorldP3D_History &data) {
        archive(data.pos);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              WorldP2D &data) {
        archive(data.value);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              GrowInTime &data) {
        archive(data.initialSize, data.maxSize, data.period, data.factor, data.elapsed);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              ParticleEmitter &data) {
        archive(data.tag, data.size, data.count, data.growth);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              MaterialTransition_Fade &data) {
        archive(data.fadeFunc, data.factor);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              AfterMark &data) {
        archive(data.tag);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              EntityAttachment &data) {
        archive(data.entities, data.entityTypes);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              EntityAttachee &data) {
        archive(data.entityID, data.entityType);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              Orientation &data) {
        archive(data.value);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              Scale &data) {
        archive(data.value);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              CarConfig &data) {
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
              Car &data) {
        archive(
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
                data.slipAngleFront,
                data.slipAngleRear,
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
              Tag_Static &) {}

    template<class Archive>
    void
    serialize(Archive &archive,
              Tag_Dynamic &) {}

    template<class Archive>
    void
    serialize(Archive &archive,
              Tag_Audible &) {}

    template<class Archive>
    void
    serialize(Archive &archive,
              Age &data) {
        archive(data.currentAge, data.maxAge);
    }

    std::string
    serialize(EntityManager &manager,
              SnapshotType snapshotType) {
        std::stringstream storage{};
        {
            cereal::PortableBinaryOutputArchive output{storage};
            manager.snapshot<
                    cereal::PortableBinaryOutputArchive,
                    Car,
                    CarConfig,
                    WorldP3D,
                    WorldP3D_History,
                    WorldP2D,
                    Orientation,
                    Scale,
                    Rectangle,
                    Point,
                    Age,
                    EntityPreset,
                    ParticleEmitter,
                    AfterMark,
                    EntityAttachment,
                    EntityAttachee,
                    Sound,

                    EntityType,
                    // TODO: The following component indicates that this needs to move to game code, but I have
                    // to think about this class and if engine should expose functionalities
                    // CarLapInfo,

                    // TODO: This is a cluster fuck of a design. This is just a raw pointer. Client doesnt need
                    // to know what it points to at the moment because sever does the physics calculations and only
                    // send the results back to the client, so I can skip it. But for the future I have to
                    // find a solution to this shit.
                    //components::PhysicalProperties,

                    Tag_Dynamic,
                    Tag_Static,
                    Tag_Audible
                            >(output, snapshotType);
        }

        return storage.str();
    }

    void
    deserialize(EntityManager &manager,
                const std::string &data,
                SnapshotType snapshotType) {
        std::stringstream storage;
        storage.str(data);

        {
            cereal::PortableBinaryInputArchive input{storage};
            manager.restore<
                    cereal::PortableBinaryInputArchive,
                    Car,
                    CarConfig,
                    WorldP3D,
                    WorldP3D_History,
                    WorldP2D,
                    Orientation,
                    Scale,
                    Rectangle,
                    Point,
                    Age,
                    EntityPreset,
                    ParticleEmitter,
                    AfterMark,
                    EntityAttachment,
                    EntityAttachee,
                    Sound,

                    EntityType,
                    // TODO: The following component indicates that this needs to move to game code, but I have
                    // to think about this class and if engine should expose functionalities
                    // CarLapInfo,

                    //components::PhysicalProperties,

                    Tag_Dynamic,
                    Tag_Static,
                    Tag_Audible
                           >(snapshotType, input,
                    // NOTE: Entities might keep references to other entities but those ids might change during
                    // client-server sync process, this will make sure that the client side does the correct
                    // mapping from client side ids to server side ids for each entity.
                             &EntityAttachment::entities,
                             &EntityAttachee::entityID
                    // TODO: same as above TODO
                    //, &CarLapInfo::entityID
            );
        }
    }
}