#include <oni-core/math/vec3.h>

#include <cmath>

namespace oni {
    namespace math {
        vec3 &vec3::add(const vec3 &other) {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        vec3 &vec3::subtract(const vec3 &other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        vec3 &vec3::multiply(const vec3 &other) {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            return *this;
        }

        vec3 &vec3::divide(const vec3 &other) {
            x /= other.x;
            y /= other.y;
            z /= other.z;
            return *this;
        }

        vec3 operator+(const vec3 &left, const vec3 &right) {
            return vec3{left.x, left.y, left.z}.add(right);
        }

        vec3 operator-(const vec3 &left, const vec3 &right) {
            return vec3{left.x, left.y, left.z}.subtract(right);
        }

        vec3 operator*(const vec3 &left, const vec3 &right) {
            return vec3{left.x, left.y, left.z}.multiply(right);
        }

        vec3 operator/(const vec3 &left, const vec3 &right) {
            return vec3{left.x, left.y, left.z}.divide(right);
        }

        vec3 &vec3::operator+=(const vec3 &other) {
            return add(other);
        }

        vec3 &vec3::operator-=(const vec3 &other) {
            return subtract(other);
        }

        vec3 &vec3::operator*=(const vec3 &other) {
            return multiply(other);
        }

        vec3 &vec3::operator/=(const vec3 &other) {
            return divide(other);
        }

        bool vec3::operator==(const vec3 &other) {
            return x == other.x && y == other.y && z == other.z;
        }

        bool vec3::operator!=(const vec3 &other) {
            return !(*this == other);
        }

        std::ostream &operator<<(std::ostream &stream, const vec3 &vector) {
            stream << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
            return stream;
        }

        vec2 vec3::getXY() const {
            return vec2{x, y};
        }

        common::real32 vec3::len() {
            return std::sqrt(x * x + y * y + z * z);
        }
    }
}
