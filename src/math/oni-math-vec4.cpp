#include <oni-core/math/oni-math-vec4.h>

#include <oni-core/math/oni-math-vec3.h>

namespace oni {
    vec4 &
    vec4::add(const vec4 &other) {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }

    vec3
    vec4::xyz() const {
        return {x, y, z};
    }

    vec4 &
    vec4::subtract(const vec4 &other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }

    vec4 &
    vec4::multiply(const vec4 &other) {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        w *= other.w;
        return *this;
    }

    vec4 &
    vec4::divide(const vec4 &other) {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        w /= other.w;
        return *this;
    }

    vec4
    operator+(const vec4 &left,
              const vec4 &right) {
        return vec4{left.x, left.y, left.z, left.w}.add(right);
    }

    vec4
    operator-(const vec4 &left,
              const vec4 &right) {
        return vec4{left.x, left.y, left.z, left.w}.subtract(right);
    }

    vec4
    operator*(const vec4 &left,
              const vec4 &right) {
        return vec4{left.x, left.y, left.z, left.w}.multiply(right);
    }

    vec4
    operator/(const vec4 &left,
              const vec4 &right) {
        return vec4{left.x, left.y, left.z, left.w}.divide(right);
    }

    vec4 &
    vec4::operator+=(const vec4 &other) {
        return add(other);
    }

    vec4 &
    vec4::operator-=(const vec4 &other) {
        return subtract(other);
    }

    vec4 &
    vec4::operator*=(const vec4 &other) {
        return multiply(other);
    }

    vec4 &
    vec4::operator/=(const vec4 &other) {
        return divide(other);
    }

    bool
    vec4::operator==(const vec4 &other) {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }

    bool
    vec4::operator!=(const vec4 &other) {
        return !(*this == other);
    }

    std::ostream &
    operator<<(std::ostream &stream,
               const vec4 &vector) {
        stream << "(" << vector.x << ", " << vector.y << vector.z << ", " << vector.w << ", " << ")";
        return stream;
    }

}
