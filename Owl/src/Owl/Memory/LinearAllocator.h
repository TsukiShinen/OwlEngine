#pragma once
#include <cstdint>

namespace Owl
{
	struct LinearAllocator
	{
		uint64_t TotalSize;
		uint64_t Allocated;
		void* Memory;
		bool OwnsMemory;
		
		LinearAllocator(uint64_t pTotalSize, void* pMemory);
		~LinearAllocator();

		void* Allocate(uint64_t pSize);
		void FreeAll();
	};
	
}
