#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

namespace oni {
	namespace math {
		inline float toRadians(float degrees) {
			return static_cast<float>(degrees * (M_PI / 180.0f));
		}
	}
}
