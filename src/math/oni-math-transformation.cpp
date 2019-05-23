#include <oni-core/math/oni-math-transformation.h>

#include <oni-core/entities/oni-entities-manager.h>

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
        localToWorldTranslation(const component::WorldP3D &reference,
                                component::Rectangle &rec) {
            localToWorldTranslation(reference, rec.A);
            localToWorldTranslation(reference, rec.B);
            localToWorldTranslation(reference, rec.C);
            localToWorldTranslation(reference, rec.D);
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

        component::Rectangle
        shapeTransformation(const math::mat4 &transformation,
                            const component::Rectangle &rec) {
            return component::Rectangle{transformation * rec.A,
                                        transformation * rec.B,
                                        transformation * rec.C,
                                        transformation * rec.D};
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

        component::WorldP3D
        localToWorldTranslation(const math::mat4 &trans,
                                const component::WorldP3D &operand) {
            auto result = trans * operand.value;
            return component::WorldP3D{result.x, result.y, result.z};
        }
    }
}