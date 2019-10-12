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

    r32
    mat4::determinant() const {
        auto e11 = elements[0 + 0 * 4];
        auto e12 = elements[1 + 0 * 4];
        auto e13 = elements[2 + 0 * 4];
        auto e14 = elements[3 + 0 * 4];

        auto e21 = elements[0 + 1 * 4];
        auto e22 = elements[1 + 1 * 4];
        auto e23 = elements[2 + 1 * 4];
        auto e24 = elements[3 + 1 * 4];

        auto e31 = elements[0 + 2 * 4];
        auto e32 = elements[1 + 2 * 4];
        auto e33 = elements[2 + 2 * 4];
        auto e34 = elements[3 + 2 * 4];

        auto e41 = elements[0 + 3 * 4];
        auto e42 = elements[1 + 3 * 4];
        auto e43 = elements[2 + 3 * 4];
        auto e44 = elements[3 + 3 * 4];

        auto d1 = (e22 * e33 * e44) + (e23 * e34 * e42) + (e24 * e32 * e43);
        d1 -= (e24 * e33 * e42) + (e23 * e32 * e44) + (e22 * e34 * e43);

        auto d2 = (e12 * e33 * e44) + (e13 * e34 * e42) + (e14 * e32 * e43);
        d2 -= (e14 * e33 * e42) + (e13 * e32 * e44) + (e12 * e34 * e43);

        auto d3 = (e12 * e23 * e44) + (e13 * e24 * e42) + (e14 * e22 * e43);
        d3 -= (e14 * e23 * e42) + (e13 * e22 * e44) + (e12 * e24 * e43);

        auto d4 = (e12 * e23 * e34) + (e13 * e24 * e32) + (e14 * e22 * e33);
        d4 -= (e14 * e23 * e32) + (e12 * e22 * e34) + (e12 * e24 * e33);

        auto d = (e11 * d1) - (e21 * d2) + (e31 * d3) - (e41 * d4);
        return d;
    }

    mat4
    mat4::inverse() const {
        auto det = determinant();
        auto inverseD = 1.f;
        if (det) {
            inverseD = 1 / det;
        } else {
            return identity();
        }

        auto e11 = elements[0 + 0 * 4];
        auto e12 = elements[1 + 0 * 4];
        auto e13 = elements[2 + 0 * 4];
        auto e14 = elements[3 + 0 * 4];

        auto e21 = elements[0 + 1 * 4];
        auto e22 = elements[1 + 1 * 4];
        auto e23 = elements[2 + 1 * 4];
        auto e24 = elements[3 + 1 * 4];

        auto e31 = elements[0 + 2 * 4];
        auto e32 = elements[1 + 2 * 4];
        auto e33 = elements[2 + 2 * 4];
        auto e34 = elements[3 + 2 * 4];

        auto e41 = elements[0 + 3 * 4];
        auto e42 = elements[1 + 3 * 4];
        auto e43 = elements[2 + 3 * 4];
        auto e44 = elements[3 + 3 * 4];

        auto a11 = -e24 * e33 * e42 + e23 * e34 * e42 + e24 * e32 * e43 - e22 * e34 * e43 - e23 * e32 * e44 +
                   e22 * e33 * e44;
        auto a12 = e14 * e33 * e42 - e13 * e34 * e42 - e14 * e32 * e43 + e12 * e34 * e43 + e13 * e32 * e44 -
                   e12 * e33 * e44;
        auto a13 = -e14 * e23 * e42 + e13 * e24 * e42 + e14 * e22 * e43 - e12 * e24 * e43 - e13 * e22 * e44 +
                   e12 * e23 * e44;
        auto a14 = e14 * e23 * e32 - e13 * e24 * e32 - e14 * e22 * e33 + e12 * e24 * e33 + e13 * e22 * e34 -
                   e12 * e23 * e34;
        auto a21 = e24 * e33 * e41 - e23 * e34 * e41 - e24 * e31 * e43 + e21 * e34 * e43 + e23 * e31 * e44 -
                   e21 * e33 * e44;
        auto a22 = -e14 * e33 * e41 + e13 * e34 * e41 + e14 * e31 * e43 - e11 * e34 * e43 - e13 * e31 * e44 +
                   e11 * e33 * e44;
        auto a23 = e14 * e23 * e41 - e13 * e24 * e41 - e14 * e21 * e43 + e11 * e24 * e43 + e13 * e21 * e44 -
                   e11 * e23 * e44;
        auto a24 = -e14 * e23 * e31 + e13 * e24 * e31 + e14 * e21 * e33 - e11 * e24 * e33 - e13 * e21 * e34 +
                   e11 * e23 * e34;
        auto a31 = -e24 * e32 * e41 + e22 * e34 * e41 + e24 * e31 * e42 - e21 * e34 * e42 - e22 * e31 * e44 +
                   e21 * e32 * e44;
        auto a32 = e14 * e32 * e41 - e12 * e34 * e41 - e14 * e31 * e42 + e11 * e34 * e42 + e12 * e31 * e44 -
                   e11 * e32 * e44;
        auto a33 = -e14 * e22 * e41 + e12 * e24 * e41 + e14 * e21 * e42 - e11 * e24 * e42 - e12 * e21 * e44 +
                   e11 * e22 * e44;
        auto a34 = e14 * e22 * e31 - e12 * e24 * e31 - e14 * e21 * e32 + e11 * e24 * e32 + e12 * e21 * e34 -
                   e11 * e22 * e34;
        auto a41 = e23 * e32 * e41 - e22 * e33 * e41 - e23 * e31 * e42 + e21 * e33 * e42 + e22 * e31 * e43 -
                   e21 * e32 * e43;
        auto a42 = -e13 * e32 * e41 + e12 * e33 * e41 + e13 * e31 * e42 - e11 * e33 * e42 - e12 * e31 * e43 +
                   e11 * e32 * e43;
        auto a43 = e13 * e22 * e41 - e12 * e23 * e41 - e13 * e21 * e42 + e11 * e23 * e42 + e12 * e21 * e43 -
                   e11 * e22 * e43;
        auto a44 = -e13 * e22 * e31 + e12 * e23 * e31 + e13 * e21 * e32 - e11 * e23 * e32 - e12 * e21 * e33 +
                   e11 * e22 * e33;

        auto result = mat4();

        result.elements[0 + 0 * 4] = a11;
        result.elements[1 + 0 * 4] = a12;
        result.elements[2 + 0 * 4] = a13;
        result.elements[3 + 0 * 4] = a14;

        result.elements[0 + 1 * 4] = a21;
        result.elements[1 + 1 * 4] = a22;
        result.elements[2 + 1 * 4] = a23;
        result.elements[3 + 1 * 4] = a24;

        result.elements[0 + 2 * 4] = a31;
        result.elements[1 + 2 * 4] = a32;
        result.elements[2 + 2 * 4] = a33;
        result.elements[3 + 2 * 4] = a34;

        result.elements[0 + 3 * 4] = a41;
        result.elements[1 + 3 * 4] = a42;
        result.elements[2 + 3 * 4] = a43;
        result.elements[3 + 3 * 4] = a44;

        return result * inverseD;
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
    mat4::multiply(r32 c) {
        for (auto &&element : elements) {
            element *= c;
        }
        return *this;
    }

    mat4 &
    mat4::operator*(r32 c) {
        return multiply(c);
    }

    mat4 &
    mat4::operator*=(const mat4 &other) {
        return multiply(other);
    }

    mat4 &
    mat4::operator*=(r32 c) {
        return multiply(c);
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
