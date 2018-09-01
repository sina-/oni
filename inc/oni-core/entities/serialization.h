#pragma once

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace entities {
        class EntityManager;

/*        template<class... Components>
        std::string serialize_(entities::EntityManager &manager) {
            std::stringstream storage;
            {
                cereal::PortableBinaryOutputArchive output{storage};
                manager.snapshot<cereal::PortableBinaryOutputArchive, Components...>(output);
            }

            return storage.str();
        }

        template<class... Component>
        std::string deserialize_(entities::EntityManager &manager, const std::string &data) {
            std::stringstream storage;
            storage.str(data);

            {
                cereal::PortableBinaryInputArchive input{storage};
                manager.restore<cereal::PortableBinaryInputArchive,
                        Component...>
                        (input);
            }
            return storage.str();
        }*/

        std::string serialize(entities::EntityManager &manager, bool delta);

        void deserialize(oni::entities::EntityManager &manager, const std::string &data, bool delta);
    }
}
