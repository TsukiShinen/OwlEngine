#pragma once

#define PI 3.14159265358979323846f
#define PI_2 (2.0f * PI)
#define HALF_PI (0.5f * PI)
#define QUARTER_PI (0.25f * PI)
#define ONE_OVER_PI (1.0f / PI)
#define ONE_OVER_TWO_PI (1.0f / PI_2
#define SQRT_TWO 1.41421356237309504880f
#define SQRT_THREE 1.73205080756887729352f
#define SQRT_ONE_OVER_TWO 0.70710678118654752440f
#define SQRT_ONE_OVER_THREE 0.57735026918962576450f
#define DEGREES_TO_RADIANS_MULTIPLIER (PI / 180.0f)
#define RADIANS_TO_DEGREES_MULTIPLIER (180.0f / PI)

#define SEC_TO_MILLISECONDS_MULTIPLIER 1000.0f

#define MILLISECONDS_TO_SEC_MULTIPLIER 0.001f

#define INFINITY 1e30f

#define FLOAT_EPSILON 1.192092896e-07f
#include <cstdint>

namespace Owl
{
	class Math
	{
	public:
		// ------------------------------------------
		// General math functions
		// ------------------------------------------
		static float Sin(float pValue);
		static float Cos(float pValue);
		static float Tan(float pValue);
		static float Acos(float pValue);
		static float Sqrt(float pValue);
		static float Abs(float pValue);
		static uint32_t Clamp(uint32_t pValue, uint32_t pMin, uint32_t pMax);
		static float Clamp(float pValue, float pMin, float pMax);

		/**
		 * Indicates if the value is a power of 2. 0 is considered _not_ a power of 2.
		 * @param pValue The value to be interpreted.
		 * @returns True if a power of 2, otherwise false.
		 */
		static bool IsPowerOf2(const int pValue) {
			return pValue != 0 && (pValue & pValue - 1) == 0;
		}

		static int Random();
		static int RandomRange(int pMin, int pMax);
		
		static float RandomFloat();
		static float RandomRangeFloat(float pMin, float pMax);

	    static float DegreesToRadians(const float pDegrees)
	    {
		    return pDegrees * DEGREES_TO_RADIANS_MULTIPLIER;
	    }

		static float RadiansToDegrees(const float pRadians)
	    {
	    	return pRadians * RADIANS_TO_DEGREES_MULTIPLIER;
	    }
	private:
		static bool s_RandSeeded;
	};
	
}
