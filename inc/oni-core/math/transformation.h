#pragma once

#include <oni-core/common/typedefs.h>
#include <oni-core/component/geometry.h>

namespace oni {
    namespace entities {
        class EntityManager;
    }

    namespace math {
        struct vec2;
        struct vec3;
        struct mat4;
    }

    namespace component {
        struct TransformParent;
    }

    namespace math {
        component::Shape
        shapeTransformation(const math::mat4 &transformation,
                            const component::Shape &shape);

        math::mat4
        createTransformation(const component::WorldP3D &position,
                             const component::Heading &rotation,
                             const component::Scale &scale);

        void
        worldToLocalTranslation(const component::WorldP3D &reference,
                                component::WorldP3D &operand);

        void
        localToWorldTranslation(const component::WorldP3D &reference,
                                vec3 &operand);

        void
        localToWorldTranslation(const component::WorldP3D &reference,
                                component::Shape &shape);

        void
        localToTextureTranslation(common::real32 ratio,
                                  component::WorldP3D &operand);

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
        void
        worldToTextureCoordinate(const oni::component::WorldP3D &reference,
                                 common::real32 ratio,
                                 oni::component::WorldP3D &operand);
    }
}