#pragma once

#include <oni-core/common/oni-common-typedef.h>

namespace oni {
    struct vec2 {
        r32 x{0.0f};
        r32 y{0.0f};

        vec2 &
        add(const vec2 &other);

        vec2 &
        subtract(const vec2 &other);

        vec2 &
        multiply(const vec2 &other);

        vec2 &
        divide(const vec2 &other);

        vec2 &
        divide(r32);

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
                  r32 right);

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

        r32
        len() const;

        void
        print() const;
    };
}
