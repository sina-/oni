#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/math/oni-math-fwd.h>

namespace oni {
    struct mat4 {
        // Column major ordering
        r32 elements[4 * 4]{};

        mat4();

        explicit mat4(r32 diag);

        const r32 *
        getArray() const;

        r32
        determinant() const;

        mat4
        inverse() const;

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

        mat4 &
        multiply(r32);

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
        operator*(r32);

        mat4 &
        operator*=(const mat4 &other);

        mat4 &
        operator*=(r32);

        void
        print();

    };
}
