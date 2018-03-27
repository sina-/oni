#include <oni-core/entities/vehicle.h>

namespace oni {
    namespace entities {
        Vehicle::Vehicle() {

        }

        void Vehicle::setCar(const entities::entityID &id, const components::Car &car) {
            mCars[id] = car;
        }

        void Vehicle::setCarConfig(const entities::entityID &id, const components::CarConfig &carConfig) {
            mCarConfigs[id] = carConfig;
        }

        components::Car &Vehicle::getCar(const entities::entityID &id) {
            return mCars[id];
        }

        const components::CarConfig &Vehicle::getCarConfig(const entities::entityID &id) const {
            return mCarConfigs[id];
        }

        entities::entityID Vehicle::_createEntity() {
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
            return World::_createEntity();
        }
    }
}