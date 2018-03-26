#include <oni-core/entities/vehicle.h>

namespace oni {
    namespace entities {
        void Vehicle::setCar(const entities::entityID &id, const components::Car &car) {
            mCars[id] = car;
        }

        void Vehicle::setCarConfig(const entities::entityID &id, const components::CarConfig &carConfig) {
            mCarConfigs[id] = carConfig;
        }
    }
}