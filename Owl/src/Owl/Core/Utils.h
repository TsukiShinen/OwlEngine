#pragma once
#include <functional>

namespace Owl
{
	template <typename T, typename... Rest>
	void HashCombine(std::size_t& pSeed, const T& pV, const Rest&... pRest)
	{
		pSeed ^= std::hash<T>{}(pV) + 0x9e3779b9 + (pSeed << 6) + (pSeed >> 2);
		(HashCombine(pSeed, pRest), ...);
	}
}
