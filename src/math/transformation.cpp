#include <oni-core/math/transformation.h>

#include <oni-core/entities/entity-manager.h>
#include <oni-core/component/hierarchy.h>
#include <oni-core/component/geometry.h>

namespace oni {
    namespace math {

        void Transformation::worldToLocalTranslation(const math::vec3 &reference, math::vec3 &operand) {
            operand.x -= reference.x;
            operand.y -= reference.y;
        }

        void Transformation::localToWorldTranslation(const math::vec3 &reference, math::vec3 &operand) {
            operand.x += reference.x;
            operand.y += reference.y;
        }

        void Transformation::localToWorldTranslation(const math::vec3 &reference, component::Shape &shape) {
            Transformation::localToWorldTranslation(reference, shape.vertexA);
            Transformation::localToWorldTranslation(reference, shape.vertexB);
            Transformation::localToWorldTranslation(reference, shape.vertexC);
            Transformation::localToWorldTranslation(reference, shape.vertexD);
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

        component::Shape Transformation::shapeTransformation(const math::mat4 &transformation,
                                                             const component::Shape &shape) {
            auto vertexA = shape.vertexA;
            auto vertexB = shape.vertexB;
            auto vertexC = shape.vertexC;
            auto vertexD = shape.vertexD;

            return component::Shape{transformation * vertexA,
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