#include <oni-core/math/transformation.h>

#include <oni-core/entities/entity-manager.h>
#include <oni-core/component/hierarchy.h>

namespace oni {
    namespace math {
        void
        worldToLocalTranslation(const component::WorldP3D &reference,
                                component::WorldP3D &operand) {
            operand.x -= reference.x;
            operand.y -= reference.y;
        }

        void
        localToWorldTranslation(const component::WorldP3D &reference,
                                vec3 &operand) {
            operand.x += reference.x;
            operand.y += reference.y;
        }

        void
        localToWorldTranslation(const component::WorldP3D &reference,
                                component::Shape &shape) {
            localToWorldTranslation(reference, shape.vertexA);
            localToWorldTranslation(reference, shape.vertexB);
            localToWorldTranslation(reference, shape.vertexC);
            localToWorldTranslation(reference, shape.vertexD);
        }

        void
        localToTextureTranslation(common::real32 ratio,
                                  component::WorldP3D &operand) {
            operand.x *= ratio;
            operand.y *= ratio;
        }

        void
        worldToTextureCoordinate(const component::WorldP3D &reference,
                                 common::real32 ratio,
                                 component::WorldP3D &operand) {
            worldToLocalTranslation(reference, operand);
            localToTextureTranslation(ratio, operand);
        }

        component::Shape
        shapeTransformation(const math::mat4 &transformation,
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

        math::mat4
        createTransformation(const component::WorldP3D &position,
                             const component::Heading &rotation,
                             const component::Scale &scale) {
            auto translationMat = math::mat4::translation(position.value);
            auto rotationMat = math::mat4::rotation(rotation.value, math::vec3{0.0f, 0.0f, 1.0f});
            auto scaleMat = math::mat4::scale(scale.value);
            auto transformation = translationMat * rotationMat * scaleMat;
            return transformation;
        }
    }
}