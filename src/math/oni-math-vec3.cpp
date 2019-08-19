#include <oni-core/math/oni-math-vec3.h>

#include <cmath>

#include <oni-core/math/oni-math-vec2.h>
#include <oni-core/math/oni-math-function.h>


namespace oni {
    vec3 &
    vec3::add(const vec3 &other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    vec3 &
    vec3::subtract(const vec3 &other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    vec3 &
    vec3::multiply(const vec3 &other) {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }

    vec3 &
    vec3::divide(const vec3 &other) {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        return *this;
    }

    vec3
    operator+(const vec3 &left,
              const vec3 &right) {
        return vec3{left.x, left.y, left.z}.add(right);
    }

    vec3
    operator-(const vec3 &left,
              const vec3 &right) {
        return vec3{left.x, left.y, left.z}.subtract(right);
    }

    vec3
    operator*(const vec3 &left,
              const vec3 &right) {
        return vec3{left.x, left.y, left.z}.multiply(right);
    }

    vec3
    operator/(const vec3 &left,
              const vec3 &right) {
        return vec3{left.x, left.y, left.z}.divide(right);
    }

    vec3 &
    vec3::operator+=(const vec3 &other) {
        return add(other);
    }

    vec3 &
    vec3::operator-=(const vec3 &other) {
        return subtract(other);
    }

    vec3 &
    vec3::operator*=(const vec3 &other) {
        return multiply(other);
    }

    vec3 &
    vec3::operator/=(const vec3 &other) {
        return divide(other);
    }

    bool
    vec3::operator==(const vec3 &other) {
        return almost_Equal(x, other.x) && almost_Equal(y, other.y) && almost_Equal(z, other.z);
    }

    bool
    vec3::operator!=(const vec3 &other) {
        return !(*this == other);
    }

    std::ostream &
    operator<<(std::ostream &stream,
               const vec3 &vector) {
        stream << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
        return stream;
    }

    vec2
    vec3::getXY() const {
        return vec2{x, y};
    }

    r32
    vec3::len() {
        return std::sqrt(x * x + y * y + z * z);
    }

    vec3
    operator/(const vec3 &left,
              r32 divisor) {
        return vec3{left.x / divisor, left.y / divisor, left.z / divisor};
    }
}
