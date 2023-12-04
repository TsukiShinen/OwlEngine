#pragma once
#include <vector>

#include "Owl/Memory/Memory.h"

namespace Owl
{
	template <typename T>
	class DArray : public std::vector<T>
	{
	public:
		void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, MemoryTagDarray); }
		void operator delete(void* pBlock) { OWL_FREE(pBlock, sizeof(pBlock), MemoryTagDarray); }
	};
}
