#include "opch.h"
#include "Memory.h"

namespace Owl
{
	Memory::Stats* Memory::s_Stats = nullptr;

	void Memory::Initialize(uint64_t* pMemoryRequirement, void* pBlock)
	{
		OWL_PROFILE_FUNCTION();
		*pMemoryRequirement = sizeof(Stats);
		if (pBlock == nullptr)
			return;

		s_Stats = static_cast<Stats*>(pBlock);
		s_Stats->AllocationCount = 0;
		OwlZeroMemory(s_Stats, sizeof(Stats));
	}

	void Memory::Shutdown()
	{
		OWL_PROFILE_FUNCTION();
		s_Stats = nullptr; 
	}

	void* Memory::OwlAllocate(const uint64_t pSize, const MemoryTag pTag)
	{
		OWL_PROFILE_FUNCTION();
		if (pTag == MemoryTagUnknown)
			OWL_CORE_WARN("OWL_ALLOCATE called using MEMORY_TAG_UNKNOWN. Re-class this allocation");

		if (s_Stats)
		{
			s_Stats->TotalAllocated += pSize;
			s_Stats->TaggedAllocations[pTag] += pSize;
			s_Stats->AllocationCount++;
		}

		// TODO: Memory alignment
		return memset(malloc(pSize), 0, pSize);
	}

	void Memory::OwlFree(void* pBlock, const uint64_t pSize, const MemoryTag pTag)
	{
		OWL_PROFILE_FUNCTION();
		if (pTag == MemoryTagUnknown)
			OWL_CORE_WARN("OWL_FREE called using MEMORY_TAG_UNKNOWN. Re-class this allocation");

		if (s_Stats)
		{
			s_Stats->TotalAllocated -= pSize;
			s_Stats->TaggedAllocations[pTag] -= pSize;
			s_Stats->AllocationCount--;
		}

		// TODO: Memory alignment
		free(pBlock);
	}

	void* Memory::OwlZeroMemory(void* pBlock, const uint64_t pSize)
	{
		OWL_PROFILE_FUNCTION();
		return memset(pBlock, 0, pSize);
	}

	void* Memory::OwlCopyMemory(void* pDestination, const void* pSource, const uint64_t pSize)
	{
		OWL_PROFILE_FUNCTION();
		return memcpy(pDestination, pSource, pSize);
	}

	void* Memory::OwlSetMemory(void* pDestination, const int pValue, const uint64_t pSize)
	{
		OWL_PROFILE_FUNCTION();
		return memset(pDestination, pValue, pSize);
	}

	char* Memory::OwlGetMemoryUsageString()
	{
		char buffer[8000] = "System memory use (tagged):\n";
		uint64_t offset = strlen(buffer);
		for (uint32_t i = 0; i < MEMORY_TAG_MAX_TAGS; i++)
		{
			char unit[4] = "XB";
			float amount = 1.f;
			auto s = s_Stats;
			if (s_Stats->TaggedAllocations[i] >= k_Gib)
			{
				unit[0] = 'G';
				amount = s_Stats->TaggedAllocations[i] / static_cast<float>(k_Gib);
			}
			else if (s_Stats->TaggedAllocations[i] >= k_Mib)
			{
				unit[0] = 'M';
				amount = s_Stats->TaggedAllocations[i] / static_cast<float>(k_Mib);
			}
			else if (s_Stats->TaggedAllocations[i] >= k_Kib)
			{
				unit[0] = 'K';
				amount = s_Stats->TaggedAllocations[i] / static_cast<float>(k_Kib);
			}
			else
			{
				unit[0] = 'B';
				unit[1] = 0;
				amount = s_Stats->TaggedAllocations[i];
			}

			offset += snprintf(buffer + offset, 8000, " %s: %.2f%s\n", k_MemoryTagStrings[i], amount, unit);
		}

		char* outString = _strdup(buffer);
		return outString;
	}
}
