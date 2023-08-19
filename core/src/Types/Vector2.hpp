#pragma once
#include "Utilities/MathUtils.hpp"

namespace IWXMVM
{
	struct Vector2
	{
		float x, y;

		Vector2() : x(0), y(0) {}
		Vector2(float x, float y) : x(x), y(y) {}

		static Vector2 const Zero;
		static Vector2 const One;
		static Vector2 const Right;
		static Vector2 const Up;

		float& operator[](int index)
		{
			return (&x)[index];
		}

		const float& operator[](int index) const
		{
			return (&x)[index];
		}

		Vector2 operator+(const Vector2& other) const
		{
			return Vector2(x + other.x, y + other.y);
		}

		Vector2 operator-(const Vector2& other) const
		{
			return Vector2(x - other.x, y - other.y);
		}

		Vector2 operator*(const Vector2& other) const
		{
			return Vector2(x * other.x, y * other.y);
		}

		Vector2 operator/(const Vector2& other) const
		{
			return Vector2(x / other.x, y / other.y);
		}

		Vector2 operator*(float scalar) const
		{
			return Vector2(x * scalar, y * scalar);
		}

		Vector2 operator/(float scalar) const
		{
			return Vector2(x / scalar, y / scalar);
		}

		Vector2& operator+=(const Vector2& other)
		{
			x += other.x;
			y += other.y;
			return *this;
		}

		Vector2& operator-=(const Vector2& other)
		{
			x -= other.x;
			y -= other.y;
			return *this;
		}

		Vector2& operator*=(const Vector2& other)
		{
			x *= other.x;
			y *= other.y;
			return *this;
		}

		Vector2& operator/=(const Vector2& other)
		{
			x /= other.x;
			y /= other.y;
			return *this;
		}

		Vector2& operator*=(float scalar)
		{
			x *= scalar;
			y *= scalar;
			return *this;
		}

		Vector2& operator/=(float scalar)
		{
			x /= scalar;
			y /= scalar;
			return *this;
		}
	};
}