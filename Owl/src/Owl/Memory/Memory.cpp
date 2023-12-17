#include "opch.h"
#include "Memory.h"

namespace Owl
{
	uint64_t Memory::TotalAllocated = 0;
	uint64_t Memory::TaggedAllocations[MEMORY_TAG_MAX_TAGS];
	uint64_t Memory::AllocationCount = 0;

	void* Memory::OwlAllocate(const uint64_t pSize, const MemoryTag pTag)
	{
		TotalAllocated += pSize;
		TaggedAllocations[pTag] += pSize;
		AllocationCount++;

		return malloc(pSize);
	}

	void Memory::OwlFree(void* pBlock, const uint64_t pSize, const MemoryTag pTag)
	{
		TotalAllocated -= pSize;
		TaggedAllocations[pTag] -= pSize;
		AllocationCount--;

		free(pBlock);
	}

	void* Memory::OwlCopyMemory(void* pDestination, const void* pSource, const uint64_t pSize)
	{
		return memcpy(pDestination, pSource, pSize);
	}

	char* Memory::OwlGetMemoryUsageString()
	{
		char buffer[8000] = "";
		uint64_t offset = strlen(buffer);

		char unit1[4] = "XB";
		float amount1 = 1.f;
		if (TotalAllocated >= k_Gib)
		{
			unit1[0] = 'G';
			amount1 = TotalAllocated / static_cast<float>(k_Gib);
		}
		else if (TotalAllocated >= k_Mib)
		{
			unit1[0] = 'M';
			amount1 = TotalAllocated / static_cast<float>(k_Mib);
		}
		else if (TotalAllocated >= k_Kib)
		{
			unit1[0] = 'K';
			amount1 = TotalAllocated / static_cast<float>(k_Kib);
		}
		else
		{
			unit1[0] = 'B';
			unit1[1] = 0;
			amount1 = TotalAllocated;
		}

		offset += snprintf(buffer + offset, 8000, "Memory Usage : %.2f%s // Number of allocations : %llu\n%s", amount1,
		                   unit1, AllocationCount, "System memory use (tagged):\n");
		for (uint32_t i = 0; i < MEMORY_TAG_MAX_TAGS; i++)
		{
			char unit[4] = "XB";
			float amount = 1.f;
			if (TaggedAllocations[i] >= k_Gib)
			{
				unit[0] = 'G';
				amount = TaggedAllocations[i] / static_cast<float>(k_Gib);
			}
			else if (TaggedAllocations[i] >= k_Mib)
			{
				unit[0] = 'M';
				amount = TaggedAllocations[i] / static_cast<float>(k_Mib);
			}
			else if (TaggedAllocations[i] >= k_Kib)
			{
				unit[0] = 'K';
				amount = TaggedAllocations[i] / static_cast<float>(k_Kib);
			}
			else
			{
				unit[0] = 'B';
				unit[1] = 0;
				amount = TaggedAllocations[i];
			}

			offset += snprintf(buffer + offset, 8000, "    %s: %.2f%s\n", k_MemoryTagStrings[i], amount, unit);
		}

		char* outString = _strdup(buffer);
		return outString;
	}
}
