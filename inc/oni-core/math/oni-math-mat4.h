#pragma once

#include <algorithm>
#include <array>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/math/oni-math-function.h>
#include <oni-core/math/oni-math-vec3.h>
#include <oni-core/math/oni-math-vec4.h>


namespace oni {
    struct mat4 {
        // https://stackoverflow.com/a/18177444
        union {
            // Column major ordering
            std::array<r32, 4 * 4> elements{};
            vec4 columns[4];
        };

        mat4();

        explicit mat4(r32 diag);

        const r32 *
        getArray() const { return &elements.front(); };

        vec3
        getPosition();

        static mat4
        identity();

        static mat4
        orthographic(r32 left,
                     r32 right,
                     r32 bottom,
                     r32 top,
                     r32 near,
                     r32 far);

        static mat4
        perspective(r32 fov,
                    r32 aspectRatio,
                    r32 near,
                    r32 far);

        static mat4
        translation(const vec3 &translation);

        static mat4
        translation(r32 x,
                    r32 y,
                    r32 z);

        /**
         * create rotation matrix
         * @param angle in radians
         * @param axis
         * @return
         */
        static mat4
        rotation(r32 angle,
                 const vec3 &axis);

        static mat4
        scale(r32 x,
              r32 y,
              r32 z);

        static mat4
        scale(const vec3 &scale);

        mat4 &
        multiply(const mat4 &other);

        vec4
        multiply(const vec4 &other) const;

        vec3
        multiply(const vec3 &other) const;

        friend mat4
        operator*(const mat4 &left,
                  const mat4 &right);

        friend vec4
        operator*(const mat4 &left,
                  const vec4 &right);

        friend vec3
        operator*(const mat4 &left,
                  const vec3 &right);

        mat4 &
        operator*=(const mat4 &other);

        friend std::ostream &
        operator<<(std::ostream &stream,
                   const mat4 &);

    };
}
