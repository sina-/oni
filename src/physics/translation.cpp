#include <oni-core/physics/translation.h>

namespace oni {
    namespace physics {

        void Translation::worldToLocal(const math::vec3 &reference, math::vec3 &operand) {
            operand.x -= reference.x;
            operand.y -= reference.y;
        }

        void Translation::localToWorld(const math::vec3 &reference, math::vec3 &operand) {
            operand.x += reference.x;
            operand.y += reference.y;
        }

        void Translation::localToWorld(const math::vec3 &reference, components::Placement &placement) {
            physics::Translation::localToWorld(reference, placement.vertexA);
            physics::Translation::localToWorld(reference, placement.vertexB);
            physics::Translation::localToWorld(reference, placement.vertexC);
            physics::Translation::localToWorld(reference, placement.vertexD);
        }

        void Translation::localToTexture(const float ratio, math::vec3 &operand) {
            operand.x *= ratio;
            operand.y *= ratio;
        }

        void Translation::worldToLocalTexture(const math::vec3 &reference, const float ratio, math::vec3 &operand) {
            Translation::worldToLocal(reference, operand);
            Translation::localToTexture(ratio, operand);
        }
    }
}