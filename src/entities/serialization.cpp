#include <oni-core/entities/serialization.h>

#include <sstream>

#include <cereal/archives/portable_binary.hpp>

#include <oni-core/entities/entity-manager.h>
#include <oni-core/components/geometry.h>
#include <oni-core/components/hierarchy.h>


namespace oni {
    namespace entities {

        std::string serialize(entities::EntityManager &manager) {
            std::stringstream storage;
            {
                cereal::PortableBinaryOutputArchive output{storage};
                manager.snapshot<cereal::PortableBinaryOutputArchive,
                        components::Car,
                        components::CarConfig,
                        components::Placement,
                        components::Chunk,
                        components::Shape,
                        components::Texture,
                        // TODO: This is a cluster fuck of a design. This is just a raw pointer. Client doesnt need
                        // to know what it points to at the moment because sever does the physics calculations and only
                        // send the results back to the client, so I can skip it. But for the future I have to
                        // find a solution to this shit.
                        //components::PhysicalProperties,
                        components::TagDynamic,
                        components::TagVehicle,
                        components::TagTextureShaded,
                        components::TagStatic,
                        components::TransformParent
                >(output);
            }

            return storage.str();
        }

        void deserialize(EntityManager &manager, const std::string &data) {
            std::stringstream storage;
            storage.str(data);

            {
                cereal::PortableBinaryInputArchive input{storage};
                manager.restore<cereal::PortableBinaryInputArchive,
                        components::Car,
                        components::CarConfig,
                        components::Placement,
                        components::Chunk,
                        components::Shape,
                        components::Texture,
                        //components::PhysicalProperties,
                        components::TagDynamic,
                        components::TagVehicle,
                        components::TagTextureShaded,
                        components::TagStatic,
                        components::TransformParent
                >(input);
            }
        }
    }
}