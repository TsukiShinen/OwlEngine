#include "Math.h"

#include <algorithm>
#include <corecrt_math.h>
#include <cstdlib>
#include <ctime>

namespace Owl
{
	bool Math::s_RandSeeded = false;

	float Math::Sin(const float pValue)
	{
		return sinf(pValue);
	}

	float Math::Cos(const float pValue)
	{
		return cosf(pValue);
	}

	float Math::Tan(const float pValue)
	{
		return tanf(pValue);
	}

	float Math::Acos(const float pValue)
	{
		return acosf(pValue);
	}

	float Math::Sqrt(const float pValue)
	{
		return sqrtf(pValue);
	}

	float Math::Abs(const float pValue)
	{
		return fabsf(pValue);
	}

	uint32_t Math::Clamp(const uint32_t pValue, const uint32_t pMin, const uint32_t pMax)
	{
		return std::clamp(pValue, pMin, pMax);
	}

	float Math::Clamp(const float pValue, const float pMin, const float pMax)
	{
		return std::clamp(pValue, pMin, pMax);
	}

	int Math::Random()
	{
		if (!s_RandSeeded)
		{
			srand(static_cast<unsigned>(std::time(nullptr)));
			s_RandSeeded = true;
		}
		return rand();
	}

	int Math::RandomRange(const int pMin, const int pMax)
	{
		if (!s_RandSeeded)
		{
			srand(static_cast<unsigned>(std::time(nullptr)));
			s_RandSeeded = true;
		}
		return rand() % (pMax - pMin + 1) + pMin;
	}

	float Math::RandomFloat()
	{
		return static_cast<float>(Random()) / static_cast<float>(RAND_MAX);
	}

	float Math::RandomRangeFloat(const float pMin, const float pMax)
	{
		return pMin + static_cast<float>(Random()) / (static_cast<float>(RAND_MAX) / (pMax - pMin));
	}
}
