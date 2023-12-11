#pragma once
#include <cstdint>

namespace Owl
{
	enum MemoryTag
	{
		MemoryTagUnknown,
		MemoryTagLinearAllocator,
		MemoryTagApplication,
		MemoryTagWindow,
		MemoryTagRenderer,
		MemoryTagJob,
		MemoryTagTexture,
		MemoryTagMaterialInstance,
		MemoryTagGame,
		MemoryTagTransform,
		MemoryTagEntity,
		MemoryTagEntityNode,
		MemoryTagScene,

		MEMORY_TAG_MAX_TAGS
	};

	class Memory
	{
	public:
		static void Initialize(uint64_t* pMemoryRequirement, void* pBlock);
		static void Shutdown();

		static void* OwlAllocate(uint64_t pSize, MemoryTag pTag);
		static void OwlFree(void* pBlock, uint64_t pSize, MemoryTag pTag);
		static void* OwlZeroMemory(void* pBlock, uint64_t pSize);
		static void* OwlCopyMemory(void* pDestination, const void* pSource, uint64_t pSize);
		static void* OwlSetMemory(void* pDestination, int pValue, uint64_t pSize);
		static char* OwlGetMemoryUsageString();
		static uint64_t GetMemoryAllocationCount() { return s_Stats != nullptr ? s_Stats->AllocationCount : 0; }

	private:
		struct Stats
		{
			uint64_t TotalAllocated;
			uint64_t TaggedAllocations[MEMORY_TAG_MAX_TAGS];
			uint64_t AllocationCount;
		};

		static Stats* s_Stats;
		static constexpr uint64_t k_Gib = 1024 * 1024 * 1024;
		static constexpr uint64_t k_Mib = 1024 * 1024;
		static constexpr uint64_t k_Kib = 1024;

		inline static const char* k_MemoryTagStrings[MEMORY_TAG_MAX_TAGS] = {
			"UNKNOWN    ",
			"LINEAR     ",
			"APPLICATION",
			"WINDOW     ",
			"RENDERER   ",
			"JOB        ",
			"TEXTURE    ",
			"MAT_INST   ",
			"GAME       ",
			"TRANSFORM  ",
			"ENTITY     ",
			"ENTITY_NODE",
			"SCENE      "
		};
	};
}

#define OWL_ALLOCATE(...) ::Owl::Memory::OwlAllocate(__VA_ARGS__)
#define OWL_FREE(...) ::Owl::Memory::OwlFree(__VA_ARGS__)
#define OWL_ZERO_MEMORY(...) ::Owl::Memory::OwlZeroMemory(__VA_ARGS__)
#define OWL_COPY_MEMORY(...) ::Owl::Memory::OwlCopyMemory(__VA_ARGS__)
#define OWL_SET_MEMORY(...) ::Owl::Memory::OwlSetMemory(__VA_ARGS__)
