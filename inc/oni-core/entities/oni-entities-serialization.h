#pragma once

#include <sstream>

#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/deque.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/map.hpp>

#include <oni-core/entities/oni-entities-manager.h>

namespace oni {
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