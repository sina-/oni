#include <oni-core/math/oni-math-transformation.h>


namespace oni {
    void
    worldToLocalTranslation(const WorldP3D &reference,
                            WorldP3D &operand) {
        operand.x -= reference.x;
        operand.y -= reference.y;
    }

    void
    localToWorldTranslation(const WorldP3D &reference,
                            vec3 &operand) {
        localToWorldTranslation(reference.x, reference.y, operand);
    }

    void
    localToWorldTranslation(r32 x,
                            r32 y,
                            vec3 &operand) {
        operand.x += x;
        operand.y += y;
    }

    void
    localToWorldTranslation(const WorldP3D &reference,
                            Rectangle &rec) {
        localToWorldTranslation(reference, rec.A);
        localToWorldTranslation(reference, rec.B);
        localToWorldTranslation(reference, rec.C);
        localToWorldTranslation(reference, rec.D);
    }

    void
    localToTextureTranslation(r32 ratio,
                              WorldP3D &operand) {
        operand.x *= ratio;
        operand.y *= ratio;
    }

    void
    worldToTextureCoordinate(const WorldP3D &reference,
                             r32 ratio,
                             WorldP3D &operand) {
        worldToLocalTranslation(reference, operand);
        localToTextureTranslation(ratio, operand);
    }

    Rectangle
    shapeTransformation(const mat4 &transformation,
                        const Rectangle &rec) {
        return Rectangle{transformation * rec.A,
                         transformation * rec.B,
                         transformation * rec.C,
                         transformation * rec.D};
    }

    mat4
    createTransformation(const WorldP3D &position,
                         const Heading &rotation) {
        auto translationMat = mat4::translation(position.value);
        auto rotationMat = mat4::rotation(rotation.value, vec3{0.0f, 0.0f, 1.0f});
        auto transformation = translationMat * rotationMat;
        return transformation;
    }

    mat4
    createTransformation(const WorldP3D &position,
                         const Heading &rotation,
                         const Scale &scale) {
        auto translationMat = mat4::translation(position.value);
        auto rotationMat = mat4::rotation(rotation.value, vec3{0.0f, 0.0f, 1.0f});
        auto scaleMat = mat4::scale(scale.value);
        auto transformation = translationMat * rotationMat * scaleMat;
        return transformation;
    }

    WorldP3D
    localToWorldTranslation(const mat4 &trans,
                            const WorldP3D &operand) {
        auto result = trans * operand.value;
        return WorldP3D{result.x, result.y, result.z};
    }
}
