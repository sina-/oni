#pragma once

#include <oni-core/common/oni-common-typedef.h>

namespace oni {
    struct vec3;

    struct vec4 {
        r32 x{0.0f};
        r32 y{0.0f};
        r32 z{0.0f};
        r32 w{0.0f};

        vec3
        xyz() const;

        vec4 &
        add(const vec4 &other);

        vec4 &
        subtract(const vec4 &other);

        vec4 &
        multiply(const vec4 &other);

        vec4 &
        divide(const vec4 &other);

        friend vec4
        operator+(const vec4 &left,
                  const vec4 &right);

        friend vec4
        operator-(const vec4 &left,
                  const vec4 &right);

        friend vec4
        operator*(const vec4 &left,
                  const vec4 &right);

        friend vec4
        operator/(const vec4 &left,
                  const vec4 &right);

        vec4 &
        operator+=(const vec4 &other);

        vec4 &
        operator-=(const vec4 &other);

        vec4 &
        operator*=(const vec4 &other);

        vec4 &
        operator/=(const vec4 &other);

        bool
        operator==(const vec4 &other);

        bool
        operator!=(const vec4 &other);

        void
        print() const;
    };
}
