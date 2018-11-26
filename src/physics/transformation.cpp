#include <oni-core/physics/transformation.h>

#include <oni-core/entities/entity-manager.h>
#include <oni-core/components/hierarchy.h>
#include <oni-core/components/geometry.h>

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

        void Transformation::localToTextureTranslation(const common::real32 ratio, math::vec3 &operand) {
            operand.x *= ratio;
            operand.y *= ratio;
        }

        void Transformation::worldToTextureCoordinate(const math::vec3 &reference, common::real32 ratio,
                                                      math::vec3 &operand) {
            Transformation::worldToLocalTranslation(reference, operand);
            Transformation::localToTextureTranslation(ratio, operand);
        }

        void Transformation::updatePlacement(entities::EntityManager &manager,
                                             common::EntityID entity,
                                             const components::Placement &placement) {
            manager.replace<components::Placement>(entity, placement);

            if (manager.has<components::TransformChildren>(entity)) {
                auto transformChildren = manager.get<components::TransformChildren>(entity);
                for (auto child: transformChildren.children) {
                    auto transformParent = manager.get<components::TransformParent>(child);
                    transformParent.transform = createTransformation(placement.position, placement.rotation,
                                                                     placement.scale);

                    updateTransformParent(manager, child, transformParent);
                }
            }
        }

        void Transformation::updateTransformParent(entities::EntityManager &manager,
                                                   common::EntityID entity,
                                                   const components::TransformParent &transformParent) {
            // TODO: This function should recurse
            manager.replace<components::TransformParent>(entity, transformParent);
            manager.accommodate<components::TagOnlyComponentUpdate>(entity);
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

        math::mat4 Transformation::createTransformation(const math::vec3 &position, const common::real32 rotation,
                                                        const math::vec3 &scale) {
            auto translationMat = math::mat4::translation(position);
            auto rotationMat = math::mat4::rotation(rotation, math::vec3{0.0f, 0.0f, 1.0f});
            auto scaleMat = math::mat4::scale(scale);
            auto transformation = translationMat * rotationMat * scaleMat;
            return transformation;
        }
    }
}