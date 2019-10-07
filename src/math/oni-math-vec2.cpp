#include <oni-core/math/oni-math-vec2.h>

#include <cmath>
#include <cstdio>


namespace oni {
    vec2 &
    vec2::add(const vec2 &other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    vec2 &
    vec2::subtract(const vec2 &other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    vec2 &
    vec2::multiply(const vec2 &other) {
        x *= other.x;
        y *= other.y;
        return *this;
    }

    vec2 &
    vec2::multiply(const r32 &other) {
        x *= other;
        y *= other;
        return *this;
    }

    vec2 &
    vec2::divide(const vec2 &other) {
        x /= other.x;
        y /= other.y;
        return *this;
    }

    vec2 &
    vec2::divide(r32 divisor) {
        x /= divisor;
        y /= divisor;
        return *this;
    }

    vec2
    operator+(const vec2 &left,
              const vec2 &right) {
        return vec2{left.x, left.y}.add(right);
    }

    vec2
    operator-(const vec2 &left,
              const vec2 &right) {
        return vec2{left.x, left.y}.subtract(right);
    }

    vec2
    operator*(const vec2 &left,
              const vec2 &right) {
        return vec2{left.x, left.y}.multiply(right);
    }

    vec2
    operator/(const vec2 &left,
              const vec2 &right) {
        return vec2{left.x, left.y}.divide(right);
    }

    vec2 &
    vec2::operator+=(const vec2 &other) {
        return add(other);
    }

    vec2 &
    vec2::operator-=(const vec2 &other) {
        return subtract(other);
    }

    vec2 &
    vec2::operator*=(const vec2 &other) {
        return multiply(other);
    }

    vec2 &
    vec2::operator/=(const vec2 &other) {
        return divide(other);
    }

    bool
    vec2::operator==(const vec2 &other) {
        return x == other.x && y == other.y;
    }

    bool
    vec2::operator!=(const vec2 &other) {
        return !(*this == other);
    }

    void
    vec2::print() const {
        printf("(%f, %f)\n", x, y);
    };

    r32
    vec2::len() const {
        return std::sqrt(x * x + y * y);
    }

    vec2
    operator/(const vec2 &left,
              r32 divisor) {
        return vec2{left.x, left.y}.divide(divisor);
    }
}
