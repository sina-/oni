#include <oni-core/physics/dynamics.h>

namespace oni {
    namespace physics {

        void Dynamics::tick(entities::VehicleEntityRepo &vehicle, const io::Input &input, float tickTime) {
            unsigned long entityIndex = 0;

            for (const auto &entity: vehicle.getEntities()) {
                if ((entity & components::DynamicComponent) == components::DynamicComponent
                    && (entity & components::PlacementComponent) == components::PlacementComponent) {
                    auto car = vehicle.getCar(entityIndex);
                    const auto &carConfig = vehicle.getCarConfig(entityIndex);

                    auto carInput = components::CarInput();

                    if (input.isPressed(GLFW_KEY_W) || input.isPressed(GLFW_KEY_UP)) {
                        // TODO: When using game-pad, this value will vary between (0.0f...1.0f)
                        carInput.throttle = 1.0f;
                    }
                    if (input.isPressed(GLFW_KEY_A) || input.isPressed(GLFW_KEY_LEFT)) {
                        carInput.left = 1.0f;
                    }
                    if (input.isPressed(GLFW_KEY_S) || input.isPressed(GLFW_KEY_DOWN)) {
                        carInput.brake = 1.0f;
                    }
                    if (input.isPressed(GLFW_KEY_D) || input.isPressed(GLFW_KEY_RIGHT)) {
                        carInput.right = 1.0f;
                    }
                    if (input.isPressed(GLFW_KEY_SPACE)) {
                        carInput.eBrake = 1.0f;
                    }

                    auto steerInput = carInput.left - carInput.right;
                    if (car.smoothSteer) {
                        car.steer = applySmoothSteer(car, steerInput, tickTime);
                    } else {
                        car.steer = steerInput;
                    }

                    if (car.safeSteer) {
                        car.steer = applySafeSteer(car, steerInput);
                    }

                    car.steerAngle = car.steer * carConfig.maxSteer;

                    tickCar(car, carConfig, carInput, tickTime);
                    vehicle.setCar(entityIndex, car);
                }
                ++entityIndex;
            }

        }
    }
}