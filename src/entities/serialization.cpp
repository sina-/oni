#include <oni-core/entities/serialization.h>

#include <sstream>

#include <cereal/archives/portable_binary.hpp>

#include <oni-core/entities/entity-manager.h>
#include <oni-core/components/geometry.h>
#include <oni-core/components/hierarchy.h>
#include <oni-core/components/visual.h>
#include <oni-core/components/world-data-status.h>


namespace oni {
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
                        components::TagDynamic,
                        components::TagVehicle,
                        components::TagTextureShaded,
                        components::TagColorShaded,
                        components::TagStatic
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
                        //components::Chunk,
                        components::Shape,
                        components::Appearance,
                        components::Texture,
                        //components::PhysicalProperties,
                        components::TransformParent,
                        components::TagDynamic,
                        components::TagVehicle,
                        components::TagTextureShaded,
                        components::TagColorShaded,
                        components::TagStatic
                >(lifeTime, input,
                        // NOTE: Car entity keeps a reference to tire entities but those ids might change during
                        // client-server sync process, this will make sure that the client side does the correct
                        // mapping from client side tire ids to server side ids for each Car.
                  &components::Car::tireFR,
                  &components::Car::tireFL,
                  &components::Car::tireRR,
                  &components::Car::tireRL);
            }
        }
    }
}