#include <oni-core/physics/translation.h>

namespace oni {
    namespace physics {

        void Translation::worldToLocal(const math::vec3 &reference, math::vec3 &operand) {
            operand.x -= reference.x;
            operand.y -= reference.y;
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