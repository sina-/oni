#include <oni-core/math/oni-math-mat4.h>

#include <cstdio>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/math/oni-math-function.h>
#include <oni-core/math/oni-math-vec3.h>
#include <oni-core/math/oni-math-vec4.h>


namespace oni {
    mat4::mat4() {}

    mat4::mat4(r32 diag) {
        elements[0 + 0 * 4] = diag;
        elements[1 + 1 * 4] = diag;
        elements[2 + 2 * 4] = diag;
        elements[3 + 3 * 4] = diag;
    }

    mat4
    mat4::identity() {
        return mat4(1.0f);
    }

    const r32 *
    mat4::getArray() const {
        return &elements[0];
    }

    mat4
    mat4::orthographic(r32 left,
                       r32 right,
                       r32 bottom,
                       r32 top,
                       r32 near,
                       r32 far) {
        mat4 result(0.0f);
        result.elements[0 + 0 * 4] = 2.0f / (right - left);
        result.elements[1 + 1 * 4] = 2.0f / (top - bottom);
        result.elements[2 + 2 * 4] = -2.0f / (far - near);

        result.elements[0 + 3 * 4] = (left + right) / (left - right);
        result.elements[1 + 3 * 4] = (bottom + top) / (bottom - top);
        result.elements[2 + 3 * 4] = (far + near) / (far - near);

        result.elements[3 + 3 * 4] = 1.0f;

        return result;
    }

    mat4
    mat4::perspective(r32 fov,
                      r32 aspectRatio,
                      r32 near,
                      r32 far) {
        mat4 result(0.0f);
        r32 q = 1.0f / tan(toRadians(0.5f * fov));
        r32 a = 1 / (q * aspectRatio);

        r32 b = (near + far) / (near - far);
        r32 c = (2.0f * near * far) / (near - far);

        result.elements[0 + 0 * 4] = a;
        result.elements[1 + 2 * 4] = q;
        result.elements[2 + 2 * 4] = b;
        result.elements[3 + 2 * 4] = -1.0f;
        result.elements[2 + 3 * 4] = c;

        return result;
    }

    mat4
    mat4::translation(const vec3 &translation) {
        mat4 result(1.0f);
        result.elements[0 + 3 * 4] = translation.x;
        result.elements[1 + 3 * 4] = translation.y;
        result.elements[2 + 3 * 4] = translation.z;

        return result;
    }

    mat4
    mat4::translation(r32 x,
                      r32 y,
                      r32 z) {
        mat4 result(1.0f);
        result.elements[0 + 3 * 4] = x;
        result.elements[1 + 3 * 4] = y;
        result.elements[2 + 3 * 4] = z;

        return result;
    }

    mat4
    mat4::rotation(r32 angle,
                   const vec3 &axis) {
        mat4 result(1.0f);
        r32 c = std::cos(angle);
        r32 s = std::sin(angle);
        r32 omc = 1.0f - c;

        r32 x = axis.x;
        r32 y = axis.y;
        r32 z = axis.z;

        result.elements[0 + 0 * 4] = x * omc + c;
        result.elements[1 + 0 * 4] = y * x * omc + z * s;
        result.elements[2 + 0 * 4] = x * y * omc - y * s;

        result.elements[0 + 1 * 4] = x * y * omc - z * s;
        result.elements[1 + 1 * 4] = y * omc + c;
        result.elements[2 + 1 * 4] = y * z * omc + x * s;

        result.elements[0 + 2 * 4] = x * z * omc + y * s;
        result.elements[1 + 2 * 4] = y * z * omc - x * s;
        result.elements[2 + 2 * 4] = z * omc + c;

        return result;
    }

    mat4
    mat4::scale(r32 x,
                r32 y,
                r32 z) {
        mat4 result(1.0f);
        result.elements[0 + 0 * 4] = x;
        result.elements[1 + 1 * 4] = y;
        result.elements[2 + 2 * 4] = z;

        return result;
    }

    mat4
    mat4::scale(const vec3 &v) {
        return scale(v.x, v.y, v.z);
    }

    mat4 &
    mat4::multiply(const mat4 &other) {
        r32 result[4 * 4];
        for (u8 i = 0; i < 4; i++) {
            for (u8 j = 0; j < 4; j++) {
                r32 sum = 0.0f;
                for (u8 k = 0; k < 4; k++) {
                    sum += elements[j + k * 4] * other.elements[k + i * 4];
                }
                result[j + i * 4] = sum;
            }
        }
        for (auto i = 0; i < 4 * 4; ++i) {
            elements[i] = result[i];
        }
        return *this;
    }

    mat4 &
    mat4::operator*=(const mat4 &other) {
        return multiply(other);
    }

    mat4
    operator*(const mat4 &left,
              const mat4 &right) {
        mat4 result = left;
        return result.multiply(right);
    }

    vec4
    operator*(const mat4 &left,
              const vec4 &right) {
        return left.multiply(right);
    }

    vec3
    operator*(const mat4 &left,
              const vec3 &right) {
        return left.multiply(right);
    }

    vec4
    mat4::multiply(const vec4 &other) const {
        return vec4{
                elements[0] * other.x + elements[4] * other.y + elements[8] * other.z + elements[12] * other.w,
                elements[1] * other.x + elements[5] * other.y + elements[9] * other.z + elements[13] * other.w,
                elements[2] * other.x + elements[6] * other.y + elements[10] * other.z + elements[14] * other.w,
                elements[3] * other.x + elements[7] * other.y + elements[11] * other.z + elements[15] * other.w
        };
    }

    vec3
    mat4::multiply(const vec3 &other) const {
        return vec3{
                elements[0] * other.x + elements[4] * other.y + elements[8] * other.z + elements[12],
                elements[1] * other.x + elements[5] * other.y + elements[9] * other.z + elements[13],
                elements[2] * other.x + elements[6] * other.y + elements[10] * other.z + elements[14]
        };
    }

    void
    mat4::print() {
        printf("| %f, %f, %f, %f |\n"
               "| %f, %f, %f, %f |\n"
               "| %f, %f, %f, %f |\n"
               "| %f, %f, %f, %f |\n",
               elements[0],
               elements[1],
               elements[2],
               elements[3],
               elements[4],
               elements[5],
               elements[6],
               elements[7],
               elements[8],
               elements[9],
               elements[10],
               elements[11],
               elements[12],
               elements[13],
               elements[14],
               elements[15]);
    }
}
