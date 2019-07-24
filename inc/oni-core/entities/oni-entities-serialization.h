#pragma once

#include <sstream>

#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/entities/oni-entities-entity.h>
#include <oni-core/network/oni-network-packet.h>
#include <oni-core/component/oni-component-visual.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-audio.h>
#include <oni-core/game/oni-game-event.h>


namespace oni {
    // NOTE: These functions can't be defined in serialization.cpp since that would make them invisible to users in
    // other compilation units such as server.cpp
    namespace network {
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
    }

    namespace math {
        template<class Archive>
        void
        serialize(Archive &archive,
                  mat4 &mat4) {
            archive(mat4.columns);
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
    }

    namespace component {
        template<class Archive>
        void
        serialize(Archive &archive,
                  WorldP3D &pos) {
            archive(pos.value);
        }

        template<class Archive>
        void
        serialize(Archive &archive,
                  Sound &data) {
            archive(data.group, data.tag);
        }
    }

    namespace game {
        template<class Archive>
        void
        serialize(Archive &archive,
                  game::CollidingEntity &collidingEntity) {
            archive(collidingEntity.entityA, collidingEntity.entityB);
        }

        template<class Archive>
        void
        serialize(Archive &archive,
                  game::Event_Collision &data) {
            archive(data.pos, data.colliding);
        }

        template<class Archive>
        void
        serialize(Archive &archive,
                  game::Event_RocketLaunch &data) {
            archive(data.pos);
        }

        template<class Archive>
        void
        serialize(Archive &archive,
                  game::Event_SoundPlay &data) {
            archive(data.pos, data.tag);
        }

        template<class Archive>
        void
        serialize(Archive &archive,
                  game::Event_SplatOnDeath &data) {
            archive(data.pos, data.scale, data.tag);
        }
    }

    namespace entities {
        class EntityManager;

        template<class Archive>
        void
        serialize(Archive &archive,
                  DeletedEntity &data) {
            archive(data.id, data.type);
        }

        std::string
        serialize(entities::EntityManager &manager,
                  entities::SnapshotType snapshotType);

        void
        deserialize(oni::entities::EntityManager &manager,
                    const std::string &data,
                    entities::SnapshotType snapshotType);

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
        deserialize(const common::u8 *data,
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
}
