#pragma once

#include <algorithm>
#include <array>
#include "functions.h"
#include "vec3.h"

namespace granite {
	namespace math {
		struct mat4 {
			// Column major ordering
			std::array<float, 4 * 4> elements;
			mat4();
			mat4(float diag);

			static mat4 identity();

			static mat4 orthographic(float left, float right, float bottom, float top, float near, float far);
			static mat4 perspective(float fov, float aspectRatio, float near, float far);

			static mat4 translation(const vec3& translation);
			static mat4 rotation(float angle, const vec3& axis);
			static mat4 scale(const vec3& scale);

			mat4 & multiply(const mat4 & other);
			friend mat4 operator*(const mat4& left, const mat4& right);
			mat4& operator*=(const mat4& other);


		};
	}
}
