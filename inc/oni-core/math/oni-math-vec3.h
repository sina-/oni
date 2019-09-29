#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/math/oni-math-fwd.h>

namespace oni {
    struct vec3 {
        r32 x{0.0f};
        r32 y{0.0f};
        r32 z{0.0f};

        vec3 &
        add(const vec3 &other);

        vec3 &
        subtract(const vec3 &other);

        vec3 &
        multiply(const vec3 &other);

        vec3 &
        divide(const vec3 &other);

        vec2
        getXY() const;

        friend vec3
        operator+(const vec3 &left,
                  const vec3 &right);

        friend vec3
        operator-(const vec3 &left,
                  const vec3 &right);

        friend vec3
        operator*(const vec3 &left,
                  const vec3 &right);

        friend vec3
        operator/(const vec3 &left,
                  const vec3 &right);

        friend vec3
        operator/(const vec3 &left,
                  r32 divisor);

        vec3 &
        operator+=(const vec3 &other);

        vec3 &
        operator-=(const vec3 &other);

        vec3 &
        operator*=(const vec3 &other);

        vec3 &
        operator/=(const vec3 &other);

        bool
        operator==(const vec3 &other);

        bool
        operator!=(const vec3 &other);
        r32
        len();

        void
        print() const;
    };
}
