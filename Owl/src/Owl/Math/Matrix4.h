#pragma once
#include "Math.h"
#include "Vector3.h"

namespace Owl
{
	struct Matrix4
	{
		static Matrix4 Identity()
		{
			Matrix4 matrix;
			matrix.Data[0] = 1.0f;
			matrix.Data[5] = 1.0f;
			matrix.Data[10] = 1.0f;
			matrix.Data[15] = 1.0f;
			return matrix;
		}

		static Matrix4 Orthographic(const float pLeft, const float pRight, const float pBottom, const float pTop,
		                            const float pNearClip, const float pFarClip)
		{
			Matrix4 matrix = Identity();

			const float lr = 1.0f / (pLeft - pRight);
			const float bt = 1.0f / (pBottom - pTop);
			const float nf = 1.0f / (pNearClip - pFarClip);

			matrix.Data[0] = -2.0f * lr;
			matrix.Data[5] = -2.0f * bt;
			matrix.Data[10] = 2.0f * nf;

			matrix.Data[12] = (pLeft + pRight) * lr;
			matrix.Data[13] = (pTop + pBottom) * bt;
			matrix.Data[14] = (pFarClip + pNearClip) * nf;
			return matrix;
		}

		static Matrix4 Perspective(const float pFovRadians, const float pAspectRatio, const float pNearClip,
		                           const float pFarClip)
		{
			const float halfTanFov = Math::Tan(pFovRadians * 0.5f);
			Matrix4 matrix;
			matrix.Data[0] = 1.0f / (pAspectRatio * halfTanFov);
			matrix.Data[5] = 1.0f / halfTanFov;
			matrix.Data[10] = -((pFarClip + pNearClip) / (pFarClip - pNearClip));
			matrix.Data[11] = -1.0f;
			matrix.Data[14] = -(2.0f * pFarClip * pNearClip / (pFarClip - pNearClip));
			return matrix;
		}

		static Matrix4 LookAt(Vector3 pPosition, Vector3 pTarget, Vector3 pUp)
		{
			Matrix4 matrix;
			Vector3 zAxis{
				pTarget.x - pPosition.x,
				pTarget.y - pPosition.y,
				pTarget.z - pPosition.z
			};

			zAxis.Normalize();
			Vector3 xAxis = zAxis.Cross(pUp).Normalized();
			Vector3 yAxis = xAxis.Cross(zAxis);

			matrix.Data[0] = xAxis.x;
			matrix.Data[1] = yAxis.x;
			matrix.Data[2] = -zAxis.x;
			matrix.Data[3] = 0;
			matrix.Data[4] = xAxis.y;
			matrix.Data[5] = yAxis.y;
			matrix.Data[6] = -zAxis.y;
			matrix.Data[7] = 0;
			matrix.Data[8] = xAxis.z;
			matrix.Data[9] = yAxis.z;
			matrix.Data[10] = -zAxis.z;
			matrix.Data[11] = 0;
			matrix.Data[12] = -xAxis.Dot(pPosition);
			matrix.Data[13] = -yAxis.Dot(pPosition);
			matrix.Data[14] = zAxis.Dot(pPosition);
			matrix.Data[15] = 1.0f;

			return matrix;
		}

		static Matrix4 Transpose(const Matrix4& pMatrix)
		{
			Matrix4 matrix = Identity();
			matrix.Data[0] = pMatrix.Data[0];
			matrix.Data[1] = pMatrix.Data[4];
			matrix.Data[2] = pMatrix.Data[8];
			matrix.Data[3] = pMatrix.Data[12];
			matrix.Data[4] = pMatrix.Data[1];
			matrix.Data[5] = pMatrix.Data[5];
			matrix.Data[6] = pMatrix.Data[9];
			matrix.Data[7] = pMatrix.Data[13];
			matrix.Data[8] = pMatrix.Data[2];
			matrix.Data[9] = pMatrix.Data[6];
			matrix.Data[10] = pMatrix.Data[10];
			matrix.Data[11] = pMatrix.Data[14];
			matrix.Data[12] = pMatrix.Data[3];
			matrix.Data[13] = pMatrix.Data[7];
			matrix.Data[14] = pMatrix.Data[11];
			matrix.Data[15] = pMatrix.Data[15];
			return matrix;
		}

		Matrix4& Transposed()
		{
			*this = Transpose(*this);
			return *this;
		}

		static Matrix4 Inverse(const Matrix4& pMatrix)
		{
			const float* m = pMatrix.Data;

			const float t0 = m[10] * m[15];
			const float t1 = m[14] * m[11];
			const float t2 = m[6] * m[15];
			const float t3 = m[14] * m[7];
			const float t4 = m[6] * m[11];
			const float t5 = m[10] * m[7];
			const float t6 = m[2] * m[15];
			const float t7 = m[14] * m[3];
			const float t8 = m[2] * m[11];
			const float t9 = m[10] * m[3];
			const float t10 = m[2] * m[7];
			const float t11 = m[6] * m[3];
			const float t12 = m[8] * m[13];
			const float t13 = m[12] * m[9];
			const float t14 = m[4] * m[13];
			const float t15 = m[12] * m[5];
			const float t16 = m[4] * m[9];
			const float t17 = m[8] * m[5];
			const float t18 = m[0] * m[13];
			const float t19 = m[12] * m[1];
			const float t20 = m[0] * m[9];
			const float t21 = m[8] * m[1];
			const float t22 = m[0] * m[5];
			const float t23 = m[4] * m[1];

			Matrix4 matrix;
			float* o = matrix.Data;

			o[0] = (t0 * m[5] + t3 * m[9] + t4 * m[13]) - (t1 * m[5] + t2 * m[9] + t5 * m[13]);
			o[1] = (t1 * m[1] + t6 * m[9] + t9 * m[13]) - (t0 * m[1] + t7 * m[9] + t8 * m[13]);
			o[2] = (t2 * m[1] + t7 * m[5] + t10 * m[13]) - (t3 * m[1] + t6 * m[5] + t11 * m[13]);
			o[3] = (t5 * m[1] + t8 * m[5] + t11 * m[9]) - (t4 * m[1] + t9 * m[5] + t10 * m[9]);

			float d = 1.0f / (m[0] * o[0] + m[4] * o[1] + m[8] * o[2] + m[12] * o[3]);

			o[0] = d * o[0];
			o[1] = d * o[1];
			o[2] = d * o[2];
			o[3] = d * o[3];
			o[4] = d * ((t1 * m[4] + t2 * m[8] + t5 * m[12]) - (t0 * m[4] + t3 * m[8] + t4 * m[12]));
			o[5] = d * ((t0 * m[0] + t7 * m[8] + t8 * m[12]) - (t1 * m[0] + t6 * m[8] + t9 * m[12]));
			o[6] = d * ((t3 * m[0] + t6 * m[4] + t11 * m[12]) - (t2 * m[0] + t7 * m[4] + t10 * m[12]));
			o[7] = d * ((t4 * m[0] + t9 * m[4] + t10 * m[8]) - (t5 * m[0] + t8 * m[4] + t11 * m[8]));
			o[8] = d * ((t12 * m[7] + t15 * m[11] + t16 * m[15]) - (t13 * m[7] + t14 * m[11] + t17 * m[15]));
			o[9] = d * ((t13 * m[3] + t18 * m[11] + t21 * m[15]) - (t12 * m[3] + t19 * m[11] + t20 * m[15]));
			o[10] = d * ((t14 * m[3] + t19 * m[7] + t22 * m[15]) - (t15 * m[3] + t18 * m[7] + t23 * m[15]));
			o[11] = d * ((t17 * m[3] + t20 * m[7] + t23 * m[11]) - (t16 * m[3] + t21 * m[7] + t22 * m[11]));
			o[12] = d * ((t14 * m[10] + t17 * m[14] + t13 * m[6]) - (t16 * m[14] + t12 * m[6] + t15 * m[10]));
			o[13] = d * ((t20 * m[14] + t12 * m[2] + t19 * m[10]) - (t18 * m[10] + t21 * m[14] + t13 * m[2]));
			o[14] = d * ((t18 * m[6] + t23 * m[14] + t15 * m[2]) - (t22 * m[14] + t14 * m[2] + t19 * m[6]));
			o[15] = d * ((t22 * m[10] + t16 * m[2] + t21 * m[6]) - (t20 * m[6] + t23 * m[10] + t17 * m[2]));

			return matrix;
		}

		Matrix4& Inversed()
		{
			*this = Inverse(*this);
			return *this;
		}

		static Matrix4 Translation(const Vector3 pPosition)
		{
			Matrix4 matrix = Identity();
			matrix.Data[12] = pPosition.x;
			matrix.Data[13] = pPosition.y;
			matrix.Data[14] = pPosition.z;
			return matrix;
		}

		static Matrix4 Scale(const Vector3 pScale)
		{
			Matrix4 matrix = Identity();
			matrix.Data[0] = pScale.x;
			matrix.Data[5] = pScale.y;
			matrix.Data[10] = pScale.z;
			return matrix;
		}

		static Matrix4 EulerX(const float pRadians)
		{
			Matrix4 matrix = Identity();
			const float cos = Math::Cos(pRadians);
			const float sin = Math::Sin(pRadians);

			matrix.Data[5] = cos;
			matrix.Data[6] = sin;
			matrix.Data[9] = -sin;
			matrix.Data[10] = cos;
			return matrix;
		}

		static Matrix4 EulerY(const float pRadians)
		{
			Matrix4 matrix = Identity();
			const float cos = Math::Cos(pRadians);
			const float sin = Math::Sin(pRadians);

			matrix.Data[0] = cos;
			matrix.Data[2] = -sin;
			matrix.Data[8] = sin;
			matrix.Data[10] = cos;
			return matrix;
		}

		static Matrix4 EulerZ(const float pRadians)
		{
			Matrix4 matrix = Identity();
			const float cos = Math::Cos(pRadians);
			const float sin = Math::Sin(pRadians);

			matrix.Data[0] = cos;
			matrix.Data[1] = sin;
			matrix.Data[4] = -sin;
			matrix.Data[5] = cos;
			return matrix;
		}

		static Matrix4 Euler(const float pXRadians, const float pYRadians, const float pZRadians)
		{
			const Matrix4 rx = EulerX(pXRadians);
			const Matrix4 ry = EulerY(pYRadians);
			const Matrix4 rz = EulerZ(pZRadians);
			return rx * ry * rz;
		}

		[[nodiscard]] Vector3 Forward() const
		{
			Vector3 forward{Data[2], Data[6], Data[10]};
			forward.Normalize();
			return forward;
		}

		[[nodiscard]] Vector3 Backward() const
		{
			Vector3 forward{-Data[2], -Data[6], -Data[10]};
			forward.Normalize();
			return forward;
		}

		[[nodiscard]] Vector3 Up() const
		{
			Vector3 forward{Data[1], Data[5], Data[9]};
			forward.Normalize();
			return forward;
		}

		[[nodiscard]] Vector3 Down() const
		{
			Vector3 forward{-Data[1], -Data[5], -Data[9]};
			forward.Normalize();
			return forward;
		}

		[[nodiscard]] Vector3 Right() const
		{
			Vector3 forward{Data[0], Data[4], Data[8]};
			forward.Normalize();
			return forward;
		}

		[[nodiscard]] Vector3 Left() const
		{
			Vector3 forward{-Data[0], -Data[4], -Data[8]};
			forward.Normalize();
			return forward;
		}

		// ========= Multiplication ==========
		Matrix4& operator*=(const Matrix4& pValue)
		{
			Matrix4 newMatrix;
			const float* matrix1Ptr = Data;
			const float* matrix2Ptr = pValue.Data;
			float* dataPtr = newMatrix.Data;

			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					*dataPtr =
						matrix1Ptr[0] * matrix2Ptr[0 + j] +
						matrix1Ptr[1] * matrix2Ptr[4 + j] +
						matrix1Ptr[2] * matrix2Ptr[8 + j] +
						matrix1Ptr[3] * matrix2Ptr[12 + j];
					dataPtr++;
				}
				matrix1Ptr += 4;
			}
			*this = newMatrix;
			return *this;
		}

		friend Matrix4 operator*(Matrix4 pValue, const Matrix4& pValue2)
		{
			pValue *= pValue2;
			return pValue;
		}

		float Data[16];
	};
}
