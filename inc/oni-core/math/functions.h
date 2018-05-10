#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include <oni-core/common/typedefs.h>

namespace oni {
	namespace math {
		inline common::real64 toRadians(common::real64 degrees) {
			return degrees * (M_PI / 180.0f);
		}
	}
}
