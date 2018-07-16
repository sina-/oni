#include <oni-core/entities/serialization.h>

#include <sstream>

#include <cereal/archives/portable_binary.hpp>

#include <oni-core/components/geometry.h>


namespace oni {
    namespace entities {

        std::string serialize(entt::DefaultRegistry &source) {
            std::stringstream storage;

            {
                cereal::PortableBinaryOutputArchive output{storage};
                source.snapshot().entities(output).component<components::Shape>(output);

            }
            return storage.str();
        }

        void deserialization(entt::DefaultRegistry &destination, std::string data) {
            std::stringstream storage;
            storage.str(data);

            {
                cereal::PortableBinaryInputArchive input{storage};
                destination.restore().entities(input).component<components::Shape>(input);
            }
        }
    }
}