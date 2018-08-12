#include <cmath>

#include <oni-core/math/vec2.h>

namespace oni {
	namespace math {
		vec2 & vec2::add(const vec2 & other)
		{
			x += other.x;
			y += other.y;
			return *this;
		}

		vec2 & vec2::subtract(const vec2 & other)
		{
			x -= other.x;
			y -= other.y;
			return *this;
		}

		vec2 & vec2::multiply(const vec2 & other)
		{
			x *= other.x;
			y *= other.y;
			return *this;
		}

		vec2 & vec2::divide(const vec2 & other)
		{
			x /= other.x;
			y /= other.y;
			return *this;
		}

		vec2 operator+(const vec2 & left, const vec2 & right)
		{
			return vec2{left.x, left.y}.add(right);
		}

		vec2 operator-(const vec2 & left, const vec2 & right)
		{
			return vec2{left.x, left.y}.subtract(right);
		}

		vec2 operator*(const vec2 & left, const vec2 & right)
		{
			return vec2{left.x, left.y}.multiply(right);
		}

		vec2 operator/(const vec2 & left, const vec2 & right)
		{
			return vec2{left.x, left.y}.divide(right);
		}

		vec2 & vec2::operator+=(const vec2 & other)
		{
			return add(other);
		}

		vec2 & vec2::operator-=(const vec2 & other)
		{
			return subtract(other);
		}

		vec2 & vec2::operator*=(const vec2 & other)
		{
			return multiply(other);
		}

		vec2 & vec2::operator/=(const vec2 & other)
		{
			return divide(other);
		}

		bool vec2::operator==(const vec2 & other)
		{
			return x == other.x && y == other.y;
		}

		bool vec2::operator!=(const vec2 & other)
		{
			return !(*this == other);
		}

		std::ostream & operator<<(std::ostream & stream, const vec2 & vector)
		{
			stream << "(" << vector.x << ", " << vector.y << ")";
			return stream;
		}

        float vec2::len() const {
            return std::sqrt(x * x + y * y);
        }

    }
}
