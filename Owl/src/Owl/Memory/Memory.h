#pragma once
#include <cstdint>

namespace Owl
{
	enum MemoryTag
	{
		MEMORY_TAG_UNKNOWN,
		MEMORY_TAG_ARRAY,
		MEMORY_TAG_DARRAY,
		MEMORY_TAG_DICT,
		MEMORY_TAG_RING_QUEUE,
		MEMORY_TAG_BST,
		MEMORY_TAG_STRING,
		MEMORY_TAG_APPLICATION,
		MEMORY_TAG_JOB,
		MEMORY_TAG_TEXTURE,
		MEMORY_TAG_MATERIAL_INSTANCE,
		MEMORY_TAG_RENDERER,
		MEMORY_TAG_GAME,
		MEMORY_TAG_TRANSFORM,
		MEMORY_TAG_ENTITY,
		MEMORY_TAG_ENTITY_NODE,
		MEMORY_TAG_SCENE,

		MEMORY_TAG_MAX_TAGS
	};

	class Memory
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void* OwlAllocate(uint64_t pSize, MemoryTag pTag);
		static void OwlFree(void* pBlock, uint64_t pSize, MemoryTag pTag);
		static void* OwlZeroMemory(void* pBlock, uint64_t pSize);
		static void* OwlCopyMemory(void* pDestination, const void* pSource, uint64_t pSize);
		static void* OwlSetMemory(void* pDestination, int pValue, uint64_t pSize);
		static char* OwlGetMemoryUsageString();

	private:
		struct Stats
		{
			uint64_t TotalAllocated;
			uint64_t TaggedAllocations[MEMORY_TAG_MAX_TAGS];
		};

		static Stats s_Stats;
		static constexpr uint64_t k_Gib = 1024 * 1024 * 1024;
		static constexpr uint64_t k_Mib = 1024 * 1024;
		static constexpr uint64_t k_Kib = 1024;

		inline static const char* k_MemoryTagStrings[MEMORY_TAG_MAX_TAGS] = {
			"UNKNOWN    ",
			"ARRAY      ",
			"DARRAY     ",
			"DICT       ",
			"RING_QUEUE ",
			"BST        ",
			"STRING     ",
			"APPLICATION",
			"JOB        ",
			"TEXTURE    ",
			"MAT_INST   ",
			"RENDERER   ",
			"GAME       ",
			"TRANSFORM  ",
			"ENTITY     ",
			"ENTITY_NODE",
			"SCENE      "};
	};
}

#define OWL_ALLOCATE(...) ::Owl::Memory::OwlAllocate(__VA_ARGS__)
#define OWL_FREE(...) ::Owl::Memory::OwlFree(__VA_ARGS__)
#define OWL_ZERO_MEMORY(...) ::Owl::Memory::OwlZeroMemory(__VA_ARGS__)
#define OWL_COPY_MEMORY(...) ::Owl::Memory::OwlCopyMemory(__VA_ARGS__)
#define OWL_SET_MEMORY(...) ::Owl::Memory::OwlSetMemory(__VA_ARGS__)
