#include <oni-core/entities/vehicle-entity-repo.h>

namespace oni {
    namespace entities {
        VehicleEntityRepo::VehicleEntityRepo() = default;

        VehicleEntityRepo::~VehicleEntityRepo()  = default;

        void VehicleEntityRepo::setCar(const common::entityID &id, const components::Car &car) {
            mCars[id] = car;
        }

        void VehicleEntityRepo::setCarConfig(const common::entityID &id, const components::CarConfig &carConfig) {
            mCarConfigs[id] = carConfig;
        }

        components::Car &VehicleEntityRepo::getCar(const common::entityID &id) {
            return mCars[id];
        }

        const components::CarConfig &VehicleEntityRepo::getCarConfig(const common::entityID &id) const {
            return mCarConfigs[id];
        }

        common::entityID VehicleEntityRepo::_createEntity() {
            if (!mFreeEntitySlots.empty()) {
                auto entity = mFreeEntitySlots.top();
                mFreeEntitySlots.pop();
                // Mark the memory location as free to over-write.
                mEntities[entity].set(components::READY);
                return entity;
            }

            auto carConfig = components::CarConfig();
            mCars.emplace_back(components::Car(carConfig));
            mCarConfigs.emplace_back(carConfig);
            return BasicEntityRepo::_createEntity();
        }
    }
}