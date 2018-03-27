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
                            input.throttle = 0.4f;
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
                            input.eBrake = 1.5f;
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
                    // TODO: Fix this mess of calculation
                    // TODO: Maybe this can happen in the vertex shader
                    position.vertexA = math::vec3(car.position.x, car.position.y, 0.0f);
                    position.vertexB = math::vec3(car.position.x, car.position.y + 0.5f, 0.0f);
                    position.vertexC = math::vec3(car.position.x + 1.0f, car.position.y + 0.5f, 0.0f);
                    position.vertexD = math::vec3(car.position.x + 1.0f, car.position.y, 0.0f);

                    auto a = position.vertexA;
                    auto b = position.vertexB;
                    auto c = position.vertexC;
                    auto d = position.vertexD;

                    auto sn = std::sin(car.heading);
                    auto cs = std::cos(car.heading);

                    auto cx = (a.x + c.x) / 2;
                    auto cy = (a.y + b.y) / 2;

                    a = math::vec3(a.x - cx, a.y - cy, a.z);
                    b = math::vec3(b.x - cx, b.y - cy, b.z);
                    c = math::vec3(c.x - cx, c.y - cy, c.z);
                    d = math::vec3(d.x - cx, d.y - cy, d.z);

                    auto aax = a.x * cs - a.y * sn;
                    auto aay = a.x * sn + a.y * cs;

                    auto bbx = b.x * cs - b.y * sn;
                    auto bby = b.x * sn + b.y * cs;

                    auto ccx = c.x * cs - c.y * sn;
                    auto ccy = c.x * sn + c.y * cs;

                    auto ddx = d.x * cs - d.y * sn;
                    auto ddy = d.x * sn + d.y * cs;

                    a = math::vec3(aax + cx, aay + cy, a.z);
                    b = math::vec3(bbx + cx, bby + cy, b.z);
                    c = math::vec3(ccx + cx, ccy + cy, c.z);
                    d = math::vec3(ddx + cx, ddy + cy, d.z);

                    position.vertexA = a;
                    position.vertexB = b;
                    position.vertexC = c;
                    position.vertexD = d;

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