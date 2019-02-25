#pragma once

#include <oni-core/common/typedefs.h>
#include <oni-core/math/mat4.h>

namespace oni {

    namespace entities {
        class EntityManager;
    }

    namespace math {
        struct vec2;

        struct vec3;
    }
    namespace component {
        struct Shape;
        struct Placement;
        struct TransformParent;
    }
    namespace math {
        class Transformation {
            Transformation() = default;

            ~Transformation() = default;

        public:

            static component::Shape shapeTransformation(const math::mat4 &transformation,
                                                         const component::Shape &shape);

            static math::mat4 createTransformation(const math::vec3 &position, common::real32 rotation,
                                                   const math::vec3 &scale);

            static void worldToLocalTranslation(const math::vec3 &reference, math::vec3 &operand);

            static void localToWorldTranslation(const math::vec3 &reference, math::vec3 &operand);

            static void localToWorldTranslation(const math::vec3 &reference, component::Shape &shape);

            static void localToTextureTranslation(common::real32 ratio, math::vec3 &operand);

            /**
             * Translates operand in the world coordinates to local coordinates of reference in texture coordinates.
             * For example, if object A is at (5, 10, 0) in the world, and object B at (1, 1, 0), given that
             * each unit of distance in game occupies 20 units of pixel in a texture, that is ratio, then point B
             * is located at (1 - 5, 10 - 1, 0 - 0) in reference to A in game units and multiply that
             * by the ratio to get the texture coordinates.
             *
             * @param reference to which the translation is applied in world coordinates
             * @param ratio each game unit is worth this many texture pixels
             * @param operand input as world coordinate and outputs as local texture coordinates
             */
            static void worldToTextureCoordinate(const math::vec3 &reference, common::real32 ratio,
                                                 math::vec3 &operand);
        };
    }
}
