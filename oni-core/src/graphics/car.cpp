#include <graphics/car.h>
#include <GLFW/glfw3.h>

namespace oni {
    namespace graphics {

        Car::Car(math::vec2 size, vec3 position,
                 math::vec4 color,
                 float rotation, float rotationSpeed)
                : DynamicSprite(size, position, color), m_Rotation(rotation),
                  m_RotationSpeed(rotationSpeed) {

        }

        void Car::update(int key, const Shader &shader) {
            switch (key) {
                case GLFW_KEY_W: {
                    m_Position += math::vec3(0.0f, 0.001f, 0.0f);
                    this->setVertexPositions(m_Size, m_Position);
                    break;
                }
                case GLFW_KEY_A: {
                    m_Position -= math::vec3(0.001f, 0.0f, 0.0f);
                    this->setVertexPositions(m_Size, m_Position);
                    break;
                }
                case GLFW_KEY_S: {
                    m_Position -= math::vec3(0.0f, 0.001f, 0.0f);
                    this->setVertexPositions(m_Size, m_Position);
                    break;
                }
                case GLFW_KEY_D:
                    m_Position += math::vec3(0.001f, 0.0f, 0.0f);
                    this->setVertexPositions(m_Size, m_Position);
                    break;

                default:
                    break;
            }
        }
    }
}
