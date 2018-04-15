#pragma once

#include <algorithm>
#include <array>

#include <oni-core/math/functions.h>
#include <oni-core/math/vec3.h>
#include <oni-core/math/vec4.h>

namespace oni {
	namespace math {
		// TODO: Take a look at GLM
		struct mat4 {
			// https://stackoverflow.com/a/18177444
			union
			{
				// Column major ordering
				std::array<float, 4 * 4> elements;
				vec4 columns[4];
			};

			mat4();
			mat4(float diag);

			const float * getArray() const { return &elements.front(); };

			static mat4 identity();

			static mat4 orthographic(float left, float right, float bottom, float top, float near, float far);
			static mat4 perspective(float fov, float aspectRatio, float near, float far);

			static mat4 translation(const vec3& translation);
			static mat4 translation(float x, float y, float z);
			/**
			 * create rotation matrix
			 * @param angle in radians
			 * @param axis
			 * @return
			 */
			static mat4 rotation(float angle, const vec3& axis);
			static mat4 scale(const vec3& scale);

			mat4 & multiply(const mat4 & other);
			vec4 multiply(const vec4 & other) const;
			vec3 multiply(const vec3 & other) const;
			friend mat4 operator*(const mat4& left, const mat4& right);
			friend vec4 operator*(const mat4& left, const vec4& right);
			friend vec3 operator*(const mat4& left, const vec3& right);
			mat4& operator*=(const mat4& other);


		};
	}
}
