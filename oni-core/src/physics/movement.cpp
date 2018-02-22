#include <physics/movement.h>

namespace oni {
    namespace physics {

        void Movement::update(entities::World &world, const graphics::Window &window) {
            unsigned long entityIndex = 0;

            for (const auto &entity: world.getEntities()) {
                if ((entity & components::DynamicComponent) == components::DynamicComponent) {
                    auto position = world.getEntityPlacement(entityIndex);

                    // TODO: Internals of IO, such as GLFW_KEY_..., should not
                    // be exposed outside.
                    switch (window.getKeyPressed()) {
                        case GLFW_KEY_W: {
                            updatePosition(position, math::vec3(0.0f, 0.001f, 0.0f));
                            break;
                        }
                        case GLFW_KEY_A: {
                            updatePosition(position, math::vec3(-0.001f, 0.0f, 0.0f));
                            break;
                        }
                        case GLFW_KEY_S: {
                            updatePosition(position, math::vec3(0.0f, -0.001f, 0.0f));
                            break;
                        }
                        case GLFW_KEY_D:
                            updatePosition(position, math::vec3(0.001f, 0.0f, 0.0f));
                            break;

                        default:
                            break;
                    }
                    world.setEntityPlacement(entityIndex, position);

                }
                ++entityIndex;
            }

        }

        void Movement::updatePosition(components::Placement &position, const math::vec3 &direction) {
            position.vertexA += direction;
            position.vertexB += direction;
            position.vertexC += direction;
            position.vertexD += direction;
        }
    }
}