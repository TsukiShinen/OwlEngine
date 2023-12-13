#pragma once
#include "Math.h"
#include "Vector2.h"
#include "Vector3.h"

namespace Owl
{
	struct Vector4
	{
		Vector4()
		{
			x = 0;
			y = 0;
			z = 0;
			w = 0;
		}
		Vector4(const float pX, const float pY, const float pZ, const float pW)
		{
			x = pX;
			y = pY;
			z = pZ;
			w = pW;
		}
		Vector4(const Vector3& pVector3, const float pW)
		{
			x = pVector3.x;
			y = pVector3.y;
			z = pVector3.w;
			w = pW;
		}
		Vector4(const Vector2& pVector2, const float pZ, const float pW)
		{
			x = pVector2.x;
			y = pVector2.y;
			z = pZ;
			w = pW;
		}

		Vector3 ToVector3()
		{
			return {x, y, z};
		}

		Vector2 ToVector2()
		{
			return {x, y};
		}

		static Vector4 Zero() { return {0.0f, 0.0f, 0.0f, 0.0f}; }
		static Vector4 One() { return {1.0f, 1.0f, 1.0f, 1.0f}; }

		[[nodiscard]] float LenghtSquared() const
		{
			return x * x + y * y + z * z + w * w;
		}

		[[nodiscard]] float Lenght() const
		{
			return Math::Sqrt(LenghtSquared());
		}

		void Normalize()
		{
			*this /= Lenght();
		}

		[[nodiscard]] float Distance(const Vector4& pValue) const
		{
			return (*this - pValue).Lenght();
		}

		[[nodiscard]] Vector4 Normalized() const
		{
			auto newVector = *this;
			newVector.Normalize();
			return newVector;
		}

		[[nodiscard]] float Dot(const Vector4& pValue) const
		{
			float p = 0;
			p += x * pValue.x;
			p += y * pValue.y;
			p += z * pValue.z;
			p += w * pValue.w;
			return p;
		}

		// ========== Addition ==========
		Vector4& operator+=(const Vector4& pValue)
		{
			x += pValue.x;
			y += pValue.y;
			z += pValue.z;
			w += pValue.w;
			return *this;
		}

		friend Vector4 operator+(Vector4 pValue, const Vector4& pValue2)
		{
			pValue += pValue2;
			return pValue;
		}
		
		// ========== Subtract ==========
		Vector4& operator-=(const Vector4& pValue)
		{
			x -= pValue.x;
			y -= pValue.y;
			z -= pValue.z;
			w-= pValue.w;
			return *this;
		}

		friend Vector4 operator-(Vector4 pValue, const Vector4& pValue2)
		{
			pValue -= pValue2;
			return pValue;
		}
		
		// ========== Multiplication ==========
		Vector4& operator*=(const Vector4& pValue)
		{
			x *= pValue.x;
			y *= pValue.y;
			z *= pValue.z;
			w *= pValue.w;
			return *this;
		}

		friend Vector4 operator*(Vector4 pValue, const Vector4& pValue2)
		{
			pValue *= pValue2;
			return pValue;
		}
		
		Vector4& operator*=(const float& pValue)
		{
			x *= pValue;
			y *= pValue;
			z *= pValue;
			w *= pValue;
			return *this;
		}

		friend Vector4 operator*(Vector4 pValue, const float& pValue2)
		{
			pValue *= pValue2;
			return pValue;
		}
		
		// ========== Divide ==========
		Vector4& operator/=(const Vector4& pValue)
		{
			x /= pValue.x;
			y /= pValue.y;
			z /= pValue.z;
			w /= pValue.w;
			return *this;
		}

		friend Vector4 operator/(Vector4 pValue, const Vector4& pValue2)
		{
			pValue /= pValue2;
			return pValue;
		}
		
		Vector4& operator/=(const float& pValue)
		{
			x /= pValue;
			y /= pValue;
			z /= pValue;
			w /= pValue;
			return *this;
		}

		friend Vector4 operator/(Vector4 pValue, const float& pValue2)
		{
			pValue *= pValue2;
			return pValue;
		}

		// ========== Equals ==========
		bool operator==(const Vector4& pValue) const
		{
			return Math::Abs(x - pValue.x) < FLOAT_EPSILON &&
					Math::Abs(y - pValue.y) < FLOAT_EPSILON &&
					Math::Abs(z - pValue.z) < FLOAT_EPSILON &&
					Math::Abs(w - pValue.w) < FLOAT_EPSILON;
		}
		
		bool operator!=(const Vector4& pValue) const
		{
			return Math::Abs(x - pValue.x) > FLOAT_EPSILON ||
					Math::Abs(y - pValue.y) > FLOAT_EPSILON ||
						Math::Abs(z - pValue.z) > FLOAT_EPSILON ||
						Math::Abs(w - pValue.w) > FLOAT_EPSILON;
		}

		struct {
			union {
				// The first element.
				float x, r, s;
			};
			union {
				// The second element.
				float y, g, t;
			};
			union {
				// The third element.
				float z, b, p;
			};
			union {
				// The third element.
				float w, a, q;
			};
		};
	};
}
