#pragma once
#include "Math.h"

namespace Owl
{
	struct Vector2
	{
		Vector2()
		{
			x = 0;
			y = 0;
		}

		Vector2(const float pX, const float pY)
		{
			x = pX;
			y = pY;
		}

		static Vector2 Zero() { return {0.0f, 0.0f}; }
		static Vector2 One() { return {1.0f, 1.0f}; }
		static Vector2 Up() { return {0.0f, 1.0f}; }
		static Vector2 Down() { return {0.0f, -1.0f}; }
		static Vector2 Left() { return {-1.0f, 0.0f}; }
		static Vector2 Right() { return {1.0f, 0.0f}; }

		[[nodiscard]] float LenghtSquared() const
		{
			return x * x + y * y;
		}

		[[nodiscard]] float Lenght() const
		{
			return Math::Sqrt(LenghtSquared());
		}

		void Normalize()
		{
			*this /= Lenght();
		}

		[[nodiscard]] Vector2 Normalized() const
		{
			auto newVector = *this;
			newVector.Normalize();
			return newVector;
		}

		[[nodiscard]] float Distance(const Vector2& pValue) const
		{
			return (*this - pValue).Lenght();
		}

		// ========== Addition ==========
		Vector2& operator+=(const Vector2& pValue)
		{
			x += pValue.x;
			y += pValue.y;
			return *this;
		}

		friend Vector2 operator+(Vector2 pValue, const Vector2& pValue2)
		{
			pValue += pValue2;
			return pValue;
		}

		// ========== Subtract ==========
		Vector2& operator-=(const Vector2& pValue)
		{
			x -= pValue.x;
			y -= pValue.y;
			return *this;
		}

		friend Vector2 operator-(Vector2 pValue, const Vector2& pValue2)
		{
			pValue -= pValue2;
			return pValue;
		}

		// ========== Multiplication ==========
		Vector2& operator*=(const Vector2& pValue)
		{
			x *= pValue.x;
			y *= pValue.y;
			return *this;
		}

		friend Vector2 operator*(Vector2 pValue, const Vector2& pValue2)
		{
			pValue *= pValue2;
			return pValue;
		}

		Vector2& operator*=(const float& pValue)
		{
			x *= pValue;
			y *= pValue;
			return *this;
		}

		friend Vector2 operator*(Vector2 pValue, const float& pValue2)
		{
			pValue *= pValue2;
			return pValue;
		}

		// ========== Divide ==========
		Vector2& operator/=(const Vector2& pValue)
		{
			x /= pValue.x;
			y /= pValue.y;
			return *this;
		}

		friend Vector2 operator/(Vector2 pValue, const Vector2& pValue2)
		{
			pValue /= pValue2;
			return pValue;
		}

		Vector2& operator/=(const float& pValue)
		{
			x /= pValue;
			y /= pValue;
			return *this;
		}

		friend Vector2 operator/(Vector2 pValue, const float& pValue2)
		{
			pValue *= pValue2;
			return pValue;
		}

		// ========== Equals ==========
		bool operator==(const Vector2& pValue) const
		{
			return Math::Abs(x - pValue.x) < FLOAT_EPSILON && Math::Abs(y - pValue.y) < FLOAT_EPSILON;
		}

		bool operator!=(const Vector2& pValue) const
		{
			return Math::Abs(x - pValue.x) > FLOAT_EPSILON || Math::Abs(y - pValue.y) > FLOAT_EPSILON;
		}

		struct
		{
			union
			{
				// The first element.
				float x, r, s, u;
			};

			union
			{
				// The second element.
				float y, g, t, v;
			};
		};
	};
}
