#pragma once

#include <algorithm>
#include <array>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/math/oni-math-function.h>
#include <oni-core/math/oni-math-vec3.h>
#include <oni-core/math/oni-math-vec4.h>

namespace oni {
    namespace math {
        struct mat4 {
            // https://stackoverflow.com/a/18177444
            union {
                // Column major ordering
                std::array<oni::common::real32, 4 * 4> elements{};
                vec4 columns[4];
            };

            mat4();

            explicit mat4(oni::common::real32 diag);

            const oni::common::real32 *
            getArray() const { return &elements.front(); };

            vec3
            getPosition();

            static mat4
            identity();

            static mat4
            orthographic(oni::common::real32 left,
                         oni::common::real32 right,
                         oni::common::real32 bottom,
                         oni::common::real32 top,
                         oni::common::real32 near,
                         oni::common::real32 far);

            static mat4
            perspective(oni::common::real32 fov,
                        oni::common::real32 aspectRatio,
                        oni::common::real32 near,
                        oni::common::real32 far);

            static mat4
            translation(const vec3 &translation);

            static mat4
            translation(oni::common::real32 x,
                        oni::common::real32 y,
                        oni::common::real32 z);

            /**
             * create rotation matrix
             * @param angle in radians
             * @param axis
             * @return
             */
            static mat4
            rotation(oni::common::real32 angle,
                     const vec3 &axis);

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


        };
    }
}
