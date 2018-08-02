#include <oni-core/entities/serialization.h>

#include <sstream>

#include <cereal/archives/portable_binary.hpp>

#include <oni-core/components/geometry.h>
#include <oni-core/components/hierarchy.h>


namespace oni {
    namespace entities {

        std::string serialize(entt::DefaultRegistry &source) {
            std::stringstream storage;
            {
                cereal::PortableBinaryOutputArchive output{storage};
                source.snapshot().entities(output).component<
                        components::Car,
                        components::CarConfig,
                        components::Placement,
                        components::Chunk,
                        components::Shape,
                        components::Texture,
                        // TODO: This is a cluster fuck of a design. This is just a raw pointer. Client doesnt need
                        // to know what it points to at the moment so I can skip it. But for the future I have to
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

        template<class... Component>
        std::string serialize_(entt::DefaultRegistry &source) {
            std::stringstream storage;
            {
                cereal::PortableBinaryOutputArchive output{storage};
                source.snapshot().entities(output).component<Component...>(output);
            }

            return storage.str();
        }

        template<class... Component>
        std::string deserialize_(entt::DefaultRegistry &destination, const std::string &data) {
            std::stringstream storage;
            storage.str(data);

            {
                cereal::PortableBinaryInputArchive input{storage};
                destination.restore().entities(input).component<Component...>(input);
            }
        }

        void deserialize(entt::DefaultRegistry &destination, const std::string &data) {
            std::stringstream storage;
            storage.str(data);

            {
                cereal::PortableBinaryInputArchive input{storage};
                destination.restore().entities(input).component<
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