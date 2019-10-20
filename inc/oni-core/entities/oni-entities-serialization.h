#pragma once

#include <sstream>

#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/deque.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/map.hpp>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/entities/oni-entities-entity.h>
#include <oni-core/entities/oni-entities-manager.h>
#include <oni-core/network/oni-network-packet.h>
#include <oni-core/component/oni-component-visual.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-audio.h>
#include <oni-core/game/oni-game-event.h>


namespace oni {
    // TODO: These all non engine components needs to move to game
    // NOTE: These functions can't be defined in serialization.cpp since that would make them invisible to users in
    // other compilation units such as server.cpp
    template<class Archive>
    void
    serialize(Archive &archive,
              Packet_Data &packet) {
        archive(packet.data);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              Packet_EntityID &packet) {
        archive(packet.entity);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              mat4 &mat4) {
        archive(mat4.elements);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              vec2 &vec2) {
        archive(vec2.x, vec2.y);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              vec3 &vec3) {
        archive(vec3.x, vec3.y, vec3.z);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              vec4 &vec4) {
        archive(vec4.x, vec4.y, vec4.z, vec4.w);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              Sound &data) {
        archive(data.group, data.tag);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              EntityPair &data) {
        archive(data.a, data.b);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              PhysicalCatPair &data) {
        archive(data.a, data.b);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              Event_Collision &data) {
        // NOTE: EntityPair is not serialized as this is server specific, although I could send it to the client
        // and do a mapping prior to use if need be!
        archive(data.pos, data.impulse, data.pcPair);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              Event_RocketLaunch &data) {
        archive(data.pos);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              Event_SoundPlay &data) {
        archive(data.pos, data.tag);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              Event_SplatOnDeath &data) {
        archive(data.pos, data.scale, data.tag);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              DeletedEntity &data) {
        archive(data.id, data.type);
    }

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
        archive(data.entities);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              EntityAttachee &data) {
        archive(data.entityID);
    }

    template<class Archive>
    void
    serialize(Archive &archive,
              EntityType &data) {
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
              TimeToLive &data) {
        archive(data.currentAge, data.maxAge);
    }

    template<class ...Components>
    std::string
    serialize(EntityManager &manager,
              SnapshotType snapshotType) {
        auto storage = std::stringstream{};
        {
            cereal::PortableBinaryOutputArchive output{storage};
            manager.snapshot<cereal::PortableBinaryOutputArchive, Components...>(output, snapshotType);
        }

        return storage.str();
    }

    template<class ...Components, class... Type, class... Member>
    void
    deserialize(oni::EntityManager &manager,
                const std::string &data,
                SnapshotType snapshotType,
                Member Type::*... member) {
        auto storage = std::stringstream{};
        storage.str(data);
        {
            cereal::PortableBinaryInputArchive input{storage};
            manager.restore<cereal::PortableBinaryInputArchive, Components...>(snapshotType, input, member...);
        }
    }

    template<class T>
    T
    deserialize(const std::string &data) {
        std::istringstream storage;
        storage.str(data);

        T result;
        {
            cereal::PortableBinaryInputArchive input{storage};
            input(result);
        }
        return result;
    }

    template<class T>
    T
    deserialize(const u8 *data,
                size_t size) {
        std::istringstream storage;
        storage.str(std::string(reinterpret_cast<const char *>(data), size));

        T result;
        {
            cereal::PortableBinaryInputArchive input{storage};
            input(result);
        }
        return result;
    }


    template<class T>
    std::string
    serialize(const T &data) {
        std::ostringstream storage;
        {
            cereal::PortableBinaryOutputArchive output{storage};

            output(data);
        }
        return storage.str();
    }
}
