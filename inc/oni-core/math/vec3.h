#pragma once

#include <iostream>

#include <oni-core/common/typedefs.h>
#include <oni-core/math/vec2.h>

namespace oni {
    namespace math {
        struct vec3 {
            common::real32 x{0.0f};
            common::real32 y{0.0f};
            common::real32 z{0.0f};

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

            friend std::ostream &
            operator<<(std::ostream &stream,
                       const vec3 &vector);

            common::real32
            len();
        };
    }
}
