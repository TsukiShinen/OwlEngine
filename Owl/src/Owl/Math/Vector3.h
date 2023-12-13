#pragma once
#include "Math.h"
#include "Vector2.h"

namespace Owl
{
	struct Vector3
	{
		Vector3()
		{
			x = 0;
			y = 0;
			z = 0;
		}
		Vector3(const float pX, const float pY, const float pZ)
		{
			x = pX;
			y = pY;
			z = pZ;
		}
		Vector3(const Vector2& pVector2, const float pZ)
		{
			x = pVector2.x;
			y = pVector2.y;
			z = pZ;
		}

		Vector2 ToVector2()
		{
			return {x, y};
		}

		static Vector3 Zero() { return {0.0f, 0.0f, 0.0f}; }
		static Vector3 One() { return {1.0f, 1.0f, 1.0f}; }
		static Vector3 Up() { return {0.0f, 1.0f, 0.0f}; }
		static Vector3 Down() { return {0.0f, -1.0f, 0.0f}; }
		static Vector3 Left() { return {-1.0f, 0.0f, 0.0f}; }
		static Vector3 Right() { return {1.0f, 0.0f, 0.0f}; }
		static Vector3 Forward() { return {0.0f, 0.0f, 1.0f}; }
		static Vector3 Backward() { return {0.0f, 0.0f, -1.0f}; }

		[[nodiscard]] float LenghtSquared() const
		{
			return x * x + y * y + z * z;
		}

		[[nodiscard]] float Lenght() const
		{
			return Math::Sqrt(LenghtSquared());
		}

		void Normalize()
		{
			*this /= Lenght();
		}

		[[nodiscard]] float Distance(const Vector3& pValue) const
		{
			return (*this - pValue).Lenght();
		}

		[[nodiscard]] Vector3 Normalized() const
		{
			auto newVector = *this;
			newVector.Normalize();
			return newVector;
		}

		[[nodiscard]] float Dot(const Vector3& pValue) const
		{
			float p = 0;
			p += x * pValue.x;
			p += y * pValue.y;
			p += z * pValue.z;
			return p;
		}
		
		[[nodiscard]] Vector3 Cross(const Vector3& pValue) const
		{
			return {y * pValue.z - z * pValue.y,
			z * pValue.x - x * pValue.z,
			x * pValue.y - y * pValue.x};
		}

		// ========== Addition ==========
		Vector3& operator+=(const Vector3& pValue)
		{
			x += pValue.x;
			y += pValue.y;
			z += pValue.z;
			return *this;
		}

		friend Vector3 operator+(Vector3 pValue, const Vector3& pValue2)
		{
			pValue += pValue2;
			return pValue;
		}
		
		// ========== Subtract ==========
		Vector3& operator-=(const Vector3& pValue)
		{
			x -= pValue.x;
			y -= pValue.y;
			z -= pValue.z;
			return *this;
		}

		friend Vector3 operator-(Vector3 pValue, const Vector3& pValue2)
		{
			pValue -= pValue2;
			return pValue;
		}
		
		// ========== Multiplication ==========
		Vector3& operator*=(const Vector3& pValue)
		{
			x *= pValue.x;
			y *= pValue.y;
			z *= pValue.z;
			return *this;
		}

		friend Vector3 operator*(Vector3 pValue, const Vector3& pValue2)
		{
			pValue *= pValue2;
			return pValue;
		}
		
		Vector3& operator*=(const float& pValue)
		{
			x *= pValue;
			y *= pValue;
			z *= pValue;
			return *this;
		}

		friend Vector3 operator*(Vector3 pValue, const float& pValue2)
		{
			pValue *= pValue2;
			return pValue;
		}
		
		// ========== Divide ==========
		Vector3& operator/=(const Vector3& pValue)
		{
			x /= pValue.x;
			y /= pValue.y;
			z /= pValue.z;
			return *this;
		}

		friend Vector3 operator/(Vector3 pValue, const Vector3& pValue2)
		{
			pValue /= pValue2;
			return pValue;
		}
		
		Vector3& operator/=(const float& pValue)
		{
			x /= pValue;
			y /= pValue;
			z /= pValue;
			return *this;
		}

		friend Vector3 operator/(Vector3 pValue, const float& pValue2)
		{
			pValue *= pValue2;
			return pValue;
		}

		// ========== Equals ==========
		bool operator==(const Vector3& pValue) const
		{
			return Math::Abs(x - pValue.x) < FLOAT_EPSILON &&
				Math::Abs(y - pValue.y) < FLOAT_EPSILON &&
				Math::Abs(z - pValue.z) < FLOAT_EPSILON;
		}
		
		bool operator!=(const Vector3& pValue) const
		{
			return Math::Abs(x - pValue.x) > FLOAT_EPSILON ||
				Math::Abs(y - pValue.y) > FLOAT_EPSILON ||
				Math::Abs(z - pValue.z) > FLOAT_EPSILON;
		}

		struct {
			union {
				// The first element.
				float x, r, s, u;
			};
			union {
				// The second element.
				float y, g, t, v;
			};
			union {
				// The third element.
				float z, b, p, w;
			};
		};
	};
}
