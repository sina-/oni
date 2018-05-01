#include <oni-core/physics/transformation.h>

namespace oni {
    namespace physics {

        void Transformation::worldToLocalTranslation(const math::vec3 &reference, math::vec3 &operand) {
            operand.x -= reference.x;
            operand.y -= reference.y;
        }

        void Transformation::localToWorldTranslation(const math::vec3 &reference, math::vec3 &operand) {
            operand.x += reference.x;
            operand.y += reference.y;
        }

        void Transformation::localToWorldTranslation(const math::vec3 &reference, components::Shape &shape) {
            physics::Transformation::localToWorldTranslation(reference, shape.vertexA);
            physics::Transformation::localToWorldTranslation(reference, shape.vertexB);
            physics::Transformation::localToWorldTranslation(reference, shape.vertexC);
            physics::Transformation::localToWorldTranslation(reference, shape.vertexD);
        }

        void Transformation::localToTextureTranslation(const float ratio, math::vec3 &operand) {
            operand.x *= ratio;
            operand.y *= ratio;
        }

        void Transformation::worldToLocalTextureTranslation(const math::vec3 &reference, const float ratio,
                                                            math::vec3 &operand) {
            Transformation::worldToLocalTranslation(reference, operand);
            Transformation::localToTextureTranslation(ratio, operand);
        }

        components::Shape Transformation::localRotation(const float rotation, const components::Shape &shape) {
            const auto centerX = (shape.vertexA.x + shape.vertexD.x) / 2.0f;
            const auto centerY = (shape.vertexA.y + shape.vertexB.y) / 2.0f;

            const auto Ax = shape.vertexA.x - centerX;
            const auto Bx = shape.vertexB.x - centerX;
            const auto Cx = shape.vertexC.x - centerX;
            const auto Dx = shape.vertexD.x - centerX;

            const auto Ay = shape.vertexA.y - centerY;
            const auto By = shape.vertexB.y - centerY;
            const auto Cy = shape.vertexC.y - centerY;
            const auto Dy = shape.vertexD.y - centerY;

            const auto cs = std::cos(rotation);
            const auto sn = std::sin(rotation);

            const auto Ax_ = static_cast<const float>(Ax * cs - Ay * sn + centerX);
            const auto Bx_ = static_cast<const float>(Bx * cs - By * sn + centerX);
            const auto Cx_ = static_cast<const float>(Cx * cs - Cy * sn + centerX);
            const auto Dx_ = static_cast<const float>(Dx * cs - Dy * sn + centerX);

            const auto Ay_ = static_cast<const float>(Ax * sn + Ay * cs + centerY);
            const auto By_ = static_cast<const float>(Bx * sn + By * cs + centerY);
            const auto Cy_ = static_cast<const float>(Cx * sn + Cy * cs + centerY);
            const auto Dy_ = static_cast<const float>(Dx * sn + Dy * cs + centerY);

            const auto A = math::vec3{Ax_, Ay_, 0.0f};
            const auto B = math::vec3{Bx_, By_, 0.0f};
            const auto C = math::vec3{Cx_, Cy_, 0.0f};
            const auto D = math::vec3{Dx_, Dy_, 0.0f};

            return components::Shape{A, B, C, D};
        }
    }
}