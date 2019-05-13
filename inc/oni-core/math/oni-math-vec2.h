#pragma once

#include <iostream>

#include <oni-core/common/oni-common-typedef.h>

namespace oni {
    namespace math {
        struct vec2 {
            common::r32 x{0.0f};
            common::r32 y{0.0f};

            vec2 &
            add(const vec2 &other);

            vec2 &
            subtract(const vec2 &other);

            vec2 &
            multiply(const vec2 &other);

            vec2 &
            divide(const vec2 &other);

            vec2 &
            divide(common::r32);

            friend vec2
            operator+(const vec2 &left,
                      const vec2 &right);

            friend vec2
            operator-(const vec2 &left,
                      const vec2 &right);

            friend vec2
            operator*(const vec2 &left,
                      const vec2 &right);

            friend vec2
            operator/(const vec2 &left,
                      const vec2 &right);

            friend vec2
            operator/(const vec2 &left,
                      common::r32 right);

            vec2 &
            operator+=(const vec2 &other);

            vec2 &
            operator-=(const vec2 &other);

            vec2 &
            operator*=(const vec2 &other);

            vec2 &
            operator/=(const vec2 &other);

            bool
            operator==(const vec2 &other);

            bool
            operator!=(const vec2 &other);

            common::r32
            len() const;

            friend std::ostream &
            operator<<(std::ostream &stream,
                       const vec2 &vector);

        };
    }
}
