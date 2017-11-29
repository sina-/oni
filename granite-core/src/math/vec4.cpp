#include "vec4.h"

namespace granite {
	namespace math {
		vec4::vec4()
		{
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
			w = 0.0f;
		}

		vec4::vec4(const float & x, const float & y, const float & z, const float & w)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}

		vec4 & vec4::add(const vec4 & other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			w += other.w;
			return *this;
		}

		vec4 & vec4::subtract(const vec4 & other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			w -= other.w;
			return *this;
		}

		vec4 & vec4::multiply(const vec4 & other)
		{
			x *= other.x;
			y *= other.y;
			z *= other.z;
			w *= other.w;
			return *this;
		}

		vec4 & vec4::divide(const vec4 & other)
		{
			x /= other.x;
			y /= other.y;
			z /= other.z;
			w /= other.w;
			return *this;
		}

		vec4 operator+(const vec4 & left, const vec4 & right)
		{
			return vec4(left.x, left.y, left.z, left.w).add(right);
		}

		vec4 operator-(const vec4 & left, const vec4 & right)
		{
			return vec4(left.x, left.y, left.z, left.w).subtract(right);
		}

		vec4 operator*(const vec4 & left, const vec4 & right)
		{
			return vec4(left.x, left.y, left.z, left.w).multiply(right);
		}

		vec4 operator/(const vec4 & left, const vec4 & right)
		{
			return vec4(left.x, left.y, left.z, left.w).divide(right);
		}

		vec4 & vec4::operator+=(const vec4 & other)
		{
			return add(other);
		}

		vec4 & vec4::operator-=(const vec4 & other)
		{
			return subtract(other);
		}

		vec4 & vec4::operator*=(const vec4 & other)
		{
			return multiply(other);
		}

		vec4 & vec4::operator/=(const vec4 & other)
		{
			return divide(other);
		}

		bool vec4::operator==(const vec4 & other)
		{
			return x == other.x && y == other.y && z == other.z && w == other.w;
		}

		bool vec4::operator!=(const vec4 & other)
		{
			return !(*this == other);
		}

		std::ostream & operator<<(std::ostream & stream, const vec4 & vector)
		{
			stream << "(" << vector.x << ", " << vector.y << vector.z << ", " << vector.w << ", " << ")";
			return stream;
		}

	}
}
