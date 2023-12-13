#include "LinearAllocator.h"

#include "Memory.h"
#include "Owl/Debug/Log.h"

namespace Owl
{
	LinearAllocator::LinearAllocator(const uint64_t pTotalSize, void* pMemory)
		: TotalSize(pTotalSize), Allocated(0), OwnsMemory(pMemory == nullptr)
	{
		if (pMemory)
			Memory = pMemory;
		else
			Memory = Memory::OwlAllocate(TotalSize, MemoryTagLinearAllocator);
	}

	LinearAllocator::~LinearAllocator()
	{
		if (OwnsMemory && Memory)
			Memory::OwlFree(Memory, TotalSize, MemoryTagLinearAllocator);
	}

	void* LinearAllocator::Allocate(uint64_t pSize)
	{
		if (Allocated + pSize > TotalSize)
		{
			OWL_CORE_ERROR("[LinearAllocator] Tried to allocate %dB, only %dB remaining.", pSize, TotalSize - Allocated);
			return nullptr;
		}

		void* block = static_cast<char*>(Memory) + Allocated;
		Allocated += pSize;
		
		return block;
	}

	void LinearAllocator::FreeAll()
	{
		Allocated = 0;
		Memory::OwlZeroMemory(Memory, TotalSize);
	}
}
