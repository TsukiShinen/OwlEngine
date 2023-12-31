﻿#pragma once
#include <cstdint>

namespace Owl
{
	enum MemoryTag
	{
		MemoryTagUnknown,
		MemoryTagApplication,
		MemoryTagPlatform,
		MemoryTagRenderer,
		MemoryTagTexture,

		MemoryTagMaxTags
	};

	struct Memory
	{
		static void* OwlAllocate(uint64_t pSize, MemoryTag pTag);
		static void OwlFree(void* pBlock, uint64_t pSize, MemoryTag pTag);
		static void* OwlCopyMemory(void* pDestination, const void* pSource, uint64_t pSize);
		static char* OwlGetMemoryUsageString();

		static uint64_t TotalAllocated;
		static uint64_t TaggedAllocations[MemoryTagMaxTags];
		static uint64_t AllocationCount;

		static constexpr uint64_t k_Gib = 1024 * 1024 * 1024;
		static constexpr uint64_t k_Mib = 1024 * 1024;
		static constexpr uint64_t k_Kib = 1024;

		inline static const char* k_MemoryTagStrings[MemoryTagMaxTags] = {
			"UNKNOWN    ",
			"APPLICATION",
			"PLATFORM   ",
			"RENDERER   ",
			"TEXTURE    ",
		};
	};
}

#define OWL_ALLOCATE(...) ::Owl::Memory::OwlAllocate(__VA_ARGS__)
#define OWL_FREE(...) ::Owl::Memory::OwlFree(__VA_ARGS__)
#define OWL_ZERO_MEMORY(...) ::Owl::Memory::OwlZeroMemory(__VA_ARGS__)
#define OWL_COPY_MEMORY(...) ::Owl::Memory::OwlCopyMemory(__VA_ARGS__)
#define OWL_SET_MEMORY(...) ::Owl::Memory::OwlSetMemory(__VA_ARGS__)
