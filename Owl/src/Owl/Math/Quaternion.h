#pragma once
#include "Matrix4.h"

namespace Owl
{
	struct Quaternion
	{
		Quaternion(const float pX, const float pY, const float pZ, const float pW)
		{
			x = pX;
			y = pY;
			z = pZ;
			w = pW;
		}
		
		Quaternion(const Vector3 pAxis, const float pAngle, const bool pNormalize)
		{
			const float halfAngle = 0.5f * pAngle;
			const float sin = Math::Sin(halfAngle);
			const float cos = Math::Cos(halfAngle);

			x = sin * pAxis.x;
			y = sin * pAxis.y;
			z = sin * pAxis.z;
			w = cos;
			if (pNormalize) 
				Normalize();
		}
		
		static Quaternion Identity()
		{
			return {0.f, 0.f, 0.f, 1.f};
		}

		[[nodiscard]] float Normal() const
		{
			return Math::Sqrt(x * x + y * y + z * z + w * w);
		}

		void Normalize()
		{
			float normal = Normal();
			x /= normal;
			y /= normal;
			z /= normal;
			w /= normal;
		}

		[[nodiscard]] Quaternion Normalized() const
		{
			auto newVector = *this;
			newVector.Normalize();
			return newVector;
		}

		void Conjugate()
		{
			x = -x;
			y = -y;
			z = -z;
		}

		[[nodiscard]] Quaternion Conjugated() const
		{
			auto newVector = *this;
			newVector.Conjugate();
			return newVector;
		}

		void Inverse()
		{
			Conjugate();
			Normalize();
		}

		[[nodiscard]] Quaternion Inversed() const
		{
			auto newVector = *this;
			newVector.Inverse();
			return newVector;
		}

		[[nodiscard]] float Dot(const Quaternion pValue) const
		{
			return x * pValue.x + y * pValue.y + z * pValue.z + w * pValue.w;
		}

		[[nodiscard]] Matrix4 ToMatrix4() const
		{
			Matrix4 matrix = Matrix4::Identity();
			const Quaternion n = Normalized();

			matrix.Data[0] = 1.0f - 2.0f * n.y * n.y - 2.0f * n.z * n.z;
			matrix.Data[1] = 2.0f * n.x * n.y - 2.0f * n.z * n.w;
			matrix.Data[2] = 2.0f * n.x * n.z + 2.0f * n.y * n.w;

			matrix.Data[4] = 2.0f * n.x * n.y + 2.0f * n.z * n.w;
			matrix.Data[5] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.z * n.z;
			matrix.Data[6] = 2.0f * n.y * n.z - 2.0f * n.x * n.w;

			matrix.Data[8] = 2.0f * n.x * n.z - 2.0f * n.y * n.w;
			matrix.Data[9] = 2.0f * n.y * n.z + 2.0f * n.x * n.w;
			matrix.Data[10] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.y * n.y;

			return matrix;
		}

		[[nodiscard]] Matrix4 ToRotationMatrix4(const Vector3 pCenter) const
		{
			Matrix4 matrix;

			float* o = matrix.Data;
			o[0] = (x * x) - (y * y) - (z * z) + (w * w);
			o[1] = 2.0f * ((x * y) + (z * w));
			o[2] = 2.0f * ((x * z) - (y * w));
			o[3] = pCenter.x - pCenter.x * o[0] - pCenter.y * o[1] - pCenter.z * o[2];

			o[4] = 2.0f * ((x * y) - (z * w));
			o[5] = -(x * x) + (y * y) - (z * z) + (w * w);
			o[6] = 2.0f * ((y * z) + (x * w));
			o[7] = pCenter.y - pCenter.x * o[4] - pCenter.y * o[5] - pCenter.z * o[6];

			o[8] = 2.0f * ((x * z) + (y * w));
			o[9] = 2.0f * ((y * z) - (x * w));
			o[10] = -(x * x) - (y * y) + (z * z) + (w * w);
			o[11] = pCenter.z - pCenter.x * o[8] - pCenter.y * o[9] - pCenter.z * o[10];

			o[12] = 0.0f;
			o[13] = 0.0f;
			o[14] = 0.0f;
			o[15] = 1.0f;
			return matrix;
		}

		static Quaternion Slerp(const Quaternion pQuaternion1, const Quaternion pQuaternion2, const float pPercentage)
		{
			const Quaternion v0 = pQuaternion1.Normalized();
			Quaternion v1 = pQuaternion2.Normalized();

			float dot = v0.Dot(v1);

			if (dot < 0.0f) {
				v1.x = -v1.x;
				v1.y = -v1.y;
				v1.z = -v1.z;
				v1.w = -v1.w;
				dot = -dot;
			}

			constexpr float dotThreshold = 0.9995f;
			if (dot > dotThreshold) {
				const Quaternion quaternion{
					v0.x + ((v1.x - v0.x) * pPercentage),
					v0.y + ((v1.y - v0.y) * pPercentage),
					v0.z + ((v1.z - v0.z) * pPercentage),
					v0.w + ((v1.w - v0.w) * pPercentage)};

				return quaternion.Normalized();
			}

			const float theta0 = Math::Acos(dot);
			const float theta = theta0 * pPercentage;
			const float sinTheta = Math::Sin(theta);
			const float sinTheta0 = Math::Sin(theta0);

			const float s0 = Math::Cos(theta) - dot * sinTheta / sinTheta0;
			const float s1 = sinTheta / sinTheta0;

			return {
				(v0.x * s0) + (v1.x * s1),
				(v0.y * s0) + (v1.y * s1),
				(v0.z * s0) + (v1.z * s1),
				(v0.w * s0) + (v1.w * s1)};
		}

		// ========== Multiplication ==========
		Quaternion& operator*=(const Quaternion& pValue)
		{
			const Quaternion quaternion{ x * pValue.w +
							   y * pValue.z -
							   z * pValue.y +
							   w * pValue.x,

							 -x * pValue.z +
							   y * pValue.w +
							   z * pValue.x +
							   w * pValue.y,

							 x * pValue.y -
							   y * pValue.x +
							   z * pValue.w +
							   w * pValue.z,

							-x * pValue.x -
							   y * pValue.y -
							   z * pValue.z +
							   w * pValue.w };

			*this = quaternion;
			return *this;
		}

		friend Quaternion operator*(Quaternion pValue, const Quaternion& pValue2)
		{
			pValue *= pValue2;
			return pValue;
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
