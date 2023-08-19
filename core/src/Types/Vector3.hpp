#pragma once
#include "Utilities/MathUtils.hpp"

namespace IWXMVM
{
	struct Vector3
	{
		float x, y, z;

		Vector3() : x(0), y(0), z(0) {}
		Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

		static Vector3 const Zero;
		static Vector3 const One;
		static Vector3 const Forward;
		static Vector3 const Right;
		static Vector3 const Up;
		static Vector3 const Down;

		float& operator[](int index)
		{
			return (&x)[index];
		}

		const float& operator[](int index) const
		{
			return (&x)[index];
		}

		Vector3 operator+(const Vector3& other) const
		{
			return Vector3(x + other.x, y + other.y, z + other.z);
		}

		Vector3 operator-(const Vector3& other) const
		{
			return Vector3(x - other.x, y - other.y, z - other.z);
		}

		Vector3 operator*(const Vector3& other) const
		{
			return Vector3(x * other.x, y * other.y, z * other.z);
		}

		Vector3 operator/(const Vector3& other) const
		{
			return Vector3(x / other.x, y / other.y, z / other.z);
		}

		Vector3 operator*(float scalar) const
		{
			return Vector3(x * scalar, y * scalar, z * scalar);
		}

		Vector3 operator/(float scalar) const
		{
			return Vector3(x / scalar, y / scalar, z / scalar);
		}

		Vector3& operator+=(const Vector3& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			return *this;
		}

		Vector3& operator-=(const Vector3& other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			return *this;
		}

		Vector3& operator*=(const Vector3& other)
		{
			x *= other.x;
			y *= other.y;
			z *= other.z;
			return *this;
		}

		Vector3& operator/=(const Vector3& other)
		{
			x /= other.x;
			y /= other.y;
			z /= other.z;
			return *this;
		}

		Vector3& operator*=(float scalar)
		{
			x *= scalar;	
		}

		Vector3& operator/=(float scalar)
		{
			x /= scalar;
		}

		static Vector3 Cross(const Vector3& a, const Vector3& b)
		{
			return Vector3(
				a.y * b.z - a.z * b.y,
				a.z * b.x - a.x * b.z,
				a.x * b.y - a.y * b.x
			);
		}

		static Vector3 FromAngles(float pitch, float yaw, float roll)
		{
			auto p = MathUtils::DegreesToRadians(pitch);
			auto y = MathUtils::DegreesToRadians(yaw);
			return Vector3(
				std::cos(y) * std::cos(p),
				std::sin(y) * std::cos(p),
				-std::sin(p)
			);
		}
	};
}