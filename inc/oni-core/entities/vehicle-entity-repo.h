#pragma once

#include <oni-core/entities/entity.h>
#include <oni-core/components/physical.h>
#include <oni-core/entities/basic-entity-repo.h>

namespace oni {
    namespace entities {
        // TODO: Make sure this inheritance doesn't add additional costs, or its minimum.
        class VehicleEntityRepo : public BasicEntityRepo {
        public:
            VehicleEntityRepo();

            ~VehicleEntityRepo();

            void setCar(const common::entityID &id, const components::Car &car);

            void setCarConfig(const common::entityID &id, const components::CarConfig &carConfig);

            components::Car &getCar(const common::entityID &id);

            const components::CarConfig &getCarConfig(const common::entityID &id) const;

        private:
            std::vector<components::Car> mCars{};
            std::vector<components::CarConfig> mCarConfigs{};

        protected:
            common::entityID _createEntity() override;

        };
    }
}
