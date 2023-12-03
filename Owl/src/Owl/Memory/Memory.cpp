#include "opch.h"
#include "Memory.h"

namespace Owl
{
	Memory::Stats Memory::s_Stats;
	
	void Memory::Initialize()
	{
		OWL_PROFILE_FUNCTION();
		memset(&s_Stats, 0, sizeof(s_Stats));
	}

	void Memory::Shutdown()
	{
		OWL_PROFILE_FUNCTION();
	}

	void* Memory::OwlAllocate(const uint64_t pSize, const MemoryTag pTag)
	{
		OWL_PROFILE_FUNCTION();
		if (pTag == MEMORY_TAG_UNKNOWN)
			OWL_CORE_WARN("OWL_ALLOCATE called using MEMORY_TAG_UNKNOWN. Re-class this allocation");

		s_Stats.TotalAllocated += pSize;
		s_Stats.TaggedAllocations[pTag] += pSize;

		// TODO: Memory alignment
		void* block = malloc(pSize);
		return memset(block, 0, pSize);
		return block;
	}

	void Memory::OwlFree(void* pBlock, const uint64_t pSize, const MemoryTag pTag)
	{
		OWL_PROFILE_FUNCTION();
		if (pTag == MEMORY_TAG_UNKNOWN)
			OWL_CORE_WARN("OWL_FREE called using MEMORY_TAG_UNKNOWN. Re-class this allocation");

		s_Stats.TotalAllocated -= pSize;
		s_Stats.TaggedAllocations[pTag] -= pSize;

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
			char unit[4] = "XiB";
			float amount = 1.f;
			if (s_Stats.TaggedAllocations[i] >= k_Gib)
			{
				unit[0] = 'G';
				amount = s_Stats.TaggedAllocations[i] / static_cast<float>(k_Gib);
			}
			else if (s_Stats.TaggedAllocations[i] >= k_Mib)
			{
				unit[0] = 'M';
				amount = s_Stats.TaggedAllocations[i] / static_cast<float>(k_Mib);
			}
			else if (s_Stats.TaggedAllocations[i] >= k_Kib)
			{
				unit[0] = 'K';
				amount = s_Stats.TaggedAllocations[i] / static_cast<float>(k_Kib);
			}
			else
			{
				unit[0] = 'B';
				unit[1] = 0;
				amount = s_Stats.TaggedAllocations[i];
			}

			offset += snprintf(buffer + offset, 8000, " %s: %.2f%s\n", k_MemoryTagStrings[i], amount, unit);
		}

		char* outString = _strdup(buffer);
		return outString;
	}
}
