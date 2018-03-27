#include <oni-core/physics/movement.h>

namespace oni {
    namespace physics {

        void Movement::update(entities::World &world, int keyPressed, float tickTime) {
            unsigned long entityIndex = 0;

            for (const auto &entity: world.getEntities()) {
                if ((entity & components::VelocityComponent) == components::VelocityComponent
                    && (entity & components::PlacementComponent) == components::PlacementComponent) {
                    auto position = world.getEntityPlacement(entityIndex);
                    auto velocity = world.getEntityVelocity(entityIndex);
                    auto magnitude = velocity.magnitude * tickTime;

                    switch (keyPressed) {
                        case GLFW_KEY_W: {
                            velocity.direction += math::vec3(0.0f, magnitude, 0.0f);
                            break;
                        }
                        case GLFW_KEY_A: {
                            velocity.direction += math::vec3(-magnitude, 0.0f, 0.0f);
                            break;
                        }
                        case GLFW_KEY_S: {
                            velocity.direction += math::vec3(0.0f, -magnitude, 0.0f);
                            break;
                        }
                        case GLFW_KEY_D:
                            velocity.direction += math::vec3(magnitude, 0.0f, 0.0f);
                            break;

                        default:
                            break;
                    }
                    updatePosition(position, velocity.direction, tickTime);
                    world.setEntityPlacement(entityIndex, position);
                    world.setEntityVelocity(entityIndex, velocity);

                }
                ++entityIndex;
            }

        }

        void Movement::update(entities::Vehicle &vehicle, int keyPressed, float tickTime) {
            unsigned long entityIndex = 0;

            for (const auto &entity: vehicle.getEntities()) {
                if ((entity & components::VelocityComponent) == components::VelocityComponent
                    && (entity & components::PlacementComponent) == components::PlacementComponent) {
                    auto position = vehicle.getEntityPlacement(entityIndex);
                    auto car = vehicle.getCar(entityIndex);
                    const auto &carConfig = vehicle.getCarConfig(entityIndex);

                    auto input = components::CarInput();

                    // TODO: Can't accept more than one key
                    switch (keyPressed) {
                        case GLFW_KEY_W: {
                            input.throttle = 1.4f;
                            break;
                        }
                        case GLFW_KEY_A: {
                            input.left = 3.3f;
                            break;
                        }
                        case GLFW_KEY_S: {
                            input.brake = 1.0f;
                            break;
                        }
                        case GLFW_KEY_D: {
                            input.right = 3.3f;
                            break;
                        }

                        case GLFW_KEY_SPACE: {
                            input.brake = 0.2f;
                            break;
                        }

                        default:
                            break;
                    }

                    auto steerInput = input.left - input.right;
                    if (car.smoothSteer) {
                        car.steer = applySmoothSteer(car, steerInput, tickTime);
                    } else {
                        car.steer = steerInput;
                    }

                    if (car.safeSteer) {
                        car.steer = applySafeSteer(car, steerInput);
                    }

                    car.steerAngle = car.steer * carConfig.maxSteer;

                    tickCar(car, carConfig, input, tickTime);

                    // TODO: Size of the car should be saved as part of Car data.
                    position.vertexA = math::vec3(car.position.x, car.position.y, 0.0f);
                    position.vertexB = math::vec3(car.position.x, car.position.y + 1.0f, 0.0f);
                    position.vertexC = math::vec3(car.position.x + 0.5f, car.position.y + 1.0f, 0.0f);
                    position.vertexD = math::vec3(car.position.x + 0.5f, car.position.y, 0.0f);

                    vehicle.setEntityPlacement(entityIndex, position);
                    vehicle.setCar(entityIndex, car);
                }
                ++entityIndex;
            }

        }

        void Movement::updatePosition(components::Placement &position, const math::vec3 &direction, float tickTime) {
            auto adjustedDirection = math::vec3(direction.x * tickTime, direction.y * tickTime, direction.z * tickTime);

            position.vertexA += adjustedDirection;
            position.vertexB += adjustedDirection;
            position.vertexC += adjustedDirection;
            position.vertexD += adjustedDirection;
        }
    }
}