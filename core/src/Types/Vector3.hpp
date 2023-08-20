#pragma once

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
			y *= scalar;
			z *= scalar;
			return *this;
		}

		Vector3& operator/=(float scalar)
		{
			x /= scalar;
			y /= scalar;
			z /= scalar;
			return *this;
		}

		bool operator==(const Vector3& other) const
		{
			return x == other.x && y == other.y && z == other.z;
		}

		Vector3 Normalized() const
		{
			float length = Length();
			if (length == 0.0f)
				return Vector3::Zero;
			return Vector3(x / length, y / length, z / length);
		}

		float Length() const
		{
			return std::sqrtf(x * x + y * y + z * z);
		}

		static Vector3 Cross(const Vector3& a, const Vector3& b)
		{
			return Vector3(
				a.y * b.z - a.z * b.y,
				a.z * b.x - a.x * b.z,
				a.x * b.y - a.y * b.x
			);
		}

		void RotateAroundAxis(const Vector3& axis, float angle)
		{
			float cosAngle = std::cosf(angle);
			float sinAngle = std::sinf(angle);
			float oneMinusCosAngle = 1.0f - cosAngle;

			this->x = (axis.x * axis.x * oneMinusCosAngle + cosAngle) * this->x + (axis.x * axis.y * oneMinusCosAngle - axis.z * sinAngle) * this->y + (axis.x * axis.z * oneMinusCosAngle + axis.y * sinAngle) * this->z;
			this->y = (axis.y * axis.x * oneMinusCosAngle + axis.z * sinAngle) * this->x + (axis.y * axis.y * oneMinusCosAngle + cosAngle) * this->y + (axis.y * axis.z * oneMinusCosAngle - axis.x * sinAngle) * this->z;
			this->z = (axis.z * axis.x * oneMinusCosAngle - axis.y * sinAngle) * this->x + (axis.z * axis.y * oneMinusCosAngle + axis.x * sinAngle) * this->y + (axis.z * axis.z * oneMinusCosAngle + cosAngle) * this->z;
		}
	};
}