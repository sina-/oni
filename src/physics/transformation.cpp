#include <oni-core/physics/transformation.h>
#include <oni-core/entities/create-entity.h>
#include <oni-core/components/hierarchy.h>

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

        void Transformation::updatePlacement(entt::DefaultRegistry &registry,
                                             entities::entityID entity,
                                             const components::Placement &placement) {
            registry.replace<components::Placement>(entity, placement);

            if (registry.has<components::TransformChildren>(entity)) {
                auto transformChildren = registry.get<components::TransformChildren>(entity);
                for (auto child: transformChildren.children) {
                    auto transformParent = registry.get<components::TransformParent>(child);
                    transformParent.transform = createTransformation(placement.position, placement.rotation,
                                                                     placement.scale);

                    updateTransformParent(registry, child, transformParent);
                }
            }
        }

        void Transformation::updateTransformParent(entt::DefaultRegistry &registry,
                                                   entities::entityID entity,
                                                   const components::TransformParent &transformParent) {
            // TODO: This function should recurse
            registry.replace<components::TransformParent>(entity, transformParent);
        }

        components::Shape Transformation::shapeTransformation(const math::mat4 &transformation,
                                                              const components::Shape &shape) {
            auto vertexA = shape.vertexA;
            auto vertexB = shape.vertexB;
            auto vertexC = shape.vertexC;
            auto vertexD = shape.vertexD;

            return components::Shape{transformation * vertexA,
                                     transformation * vertexB,
                                     transformation * vertexC,
                                     transformation * vertexD};
        }

        math::mat4 Transformation::createTransformation(const math::vec3 &position, const float rotation,
                                                        const math::vec3 &scale) {
            auto translationMat = math::mat4::translation(position);
            auto rotationMat = math::mat4::rotation(rotation, math::vec3{0.0f, 0.0f, 1.0f});
            auto scaleMat = math::mat4::scale(scale);
            auto transformation = translationMat * rotationMat * scaleMat;
            return transformation;
        }
    }
}