#include <oni-core/math/oni-math-transformation.h>

#include <oni-core/entities/oni-entities-manager.h>
#include <oni-core/component/oni-component-hierarchy.h>

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
            localToWorldTranslation(reference.x, reference.y, operand);
        }

        void
        localToWorldTranslation(common::r32 x,
                                common::r32 y,
                                vec3 &operand) {
            operand.x += x;
            operand.y += y;
        }

        void
        localToWorldTranslation(common::r32 x,
                                common::r32 y,
                                component::Shape &shape) {
            localToWorldTranslation(x, y, shape.vertexA);
            localToWorldTranslation(x, y, shape.vertexB);
            localToWorldTranslation(x, y, shape.vertexC);
            localToWorldTranslation(x, y, shape.vertexD);
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
        localToTextureTranslation(common::r32 ratio,
                                  component::WorldP3D &operand) {
            operand.x *= ratio;
            operand.y *= ratio;
        }

        void
        worldToTextureCoordinate(const component::WorldP3D &reference,
                                 common::r32 ratio,
                                 component::WorldP3D &operand) {
            worldToLocalTranslation(reference, operand);
            localToTextureTranslation(ratio, operand);
        }

        component::Shape
        shapeTransformation(const math::mat4 &transformation,
                            const component::Shape &shape) {
            return component::Shape{transformation * shape.vertexA,
                                    transformation * shape.vertexB,
                                    transformation * shape.vertexC,
                                    transformation * shape.vertexD};
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