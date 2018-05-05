#pragma once

#include <oni-core/components/geometry.h>
#include <oni-core/entities/create-entity.h>
#include <oni-core/components/hierarchy.h>

namespace oni {
    namespace math {
        class vec2;

        class vec3;
    }
    namespace components {
        class Shape;
    }
    namespace physics {
        class Transformation {
            Transformation() = default;

            ~Transformation() = default;

        public:

            static components::Shape shapeTransformation(const math::mat4 &transformation,
                                                         const components::Shape &shape);

            static math::mat4 createTransformation(const math::vec3 &position, const float rotation,
                                                   const math::vec3 &scale);

            static void worldToLocalTranslation(const math::vec3 &reference, math::vec3 &operand);

            static void localToWorldTranslation(const math::vec3 &reference, math::vec3 &operand);

            static void localToWorldTranslation(const math::vec3 &reference, components::Shape &shape);

            static void localToTextureTranslation(const float ratio, math::vec3 &operand);

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
            static void worldToLocalTextureTranslation(const math::vec3 &reference, const float ratio,
                                                       math::vec3 &operand);

            static void updatePlacement(entt::DefaultRegistry &registry,
                                        entities::entityID entity,
                                        const components::Placement &placement);

            static void updateTransformParent(entt::DefaultRegistry &registry,
                                              entities::entityID entity,
                                              const components::TransformParent &transformParent);
        };
    }
}
