#pragma once

#include <iostream>

namespace oni {
    namespace math {
        struct vec4 {
            float x{0.0f};
            float y{0.0f};
            float z{0.0f};
            float w{0.0f};

            vec4 &add(const vec4 &other);

            vec4 &subtract(const vec4 &other);

            vec4 &multiply(const vec4 &other);

            vec4 &divide(const vec4 &other);

            friend vec4 operator+(const vec4 &left, const vec4 &right);

            friend vec4 operator-(const vec4 &left, const vec4 &right);

            friend vec4 operator*(const vec4 &left, const vec4 &right);

            friend vec4 operator/(const vec4 &left, const vec4 &right);

            vec4 &operator+=(const vec4 &other);

            vec4 &operator-=(const vec4 &other);

            vec4 &operator*=(const vec4 &other);

            vec4 &operator/=(const vec4 &other);

            bool operator==(const vec4 &other);

            bool operator!=(const vec4 &other);

            friend std::ostream &operator<<(std::ostream &stream, const vec4 &vector);

        };
    }
}
