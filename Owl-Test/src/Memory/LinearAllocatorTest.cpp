#include "LinearAllocatorTest.h"

#include "../Expect.h"
#include "Owl/Memory/LinearAllocator.h"

void LinearAllocatorTest::Tests(TestManager& pTestManager)
{
	pTestManager.RegisterTest(SingleAllocationAllSpace, "Linear allocator multi allocation for all space.");
	pTestManager.RegisterTest(MultiAllocationAllSpace, "Linear allocator multi allocation for all space.");
	pTestManager.RegisterTest(MultiAllocationOverAllocate, "Linear allocator try over allocate.");
	pTestManager.RegisterTest(MultiAllocationAllSpaceThenFree, "Linear allocator allocated should be 0 after FreeAll.");
}

char LinearAllocatorTest::SingleAllocationAllSpace()
{
	Owl::LinearAllocator allocator{sizeof(uint64_t), nullptr};

	ExpectShouldNotBe(nullptr, allocator.Memory)
	ExpectShouldBe(sizeof(uint64_t), allocator.TotalSize)
	ExpectShouldBe(0, allocator.Allocated)

	return true;
}

char LinearAllocatorTest::MultiAllocationAllSpace()
{
	constexpr uint64_t maxAllocations = 1024;
	Owl::LinearAllocator allocator{sizeof(uint64_t) * maxAllocations, nullptr};

	for (uint64_t i = 0; i < maxAllocations; ++i)
	{
		void* block = allocator.Allocate(sizeof(uint64_t));

		ExpectShouldNotBe(nullptr, block)
		ExpectShouldBe(sizeof(uint64_t) * (i + 1), allocator.Allocated)
	}

	return true;
}

char LinearAllocatorTest::MultiAllocationOverAllocate()
{
	constexpr uint64_t maxAllocations = 3;
	Owl::LinearAllocator allocator{sizeof(uint64_t) * maxAllocations, nullptr};

	for (uint64_t i = 0; i < maxAllocations; ++i)
	{
		void* block = allocator.Allocate(sizeof(uint64_t));

		ExpectShouldNotBe(nullptr, block)
		ExpectShouldBe(sizeof(uint64_t) * (i + 1), allocator.Allocated)
	}

	OWL_TRACE("Note : The following error is intentionally caused by this test.");
	
	void* block = allocator.Allocate(sizeof(uint64_t));

	ExpectShouldNotBe(nullptr, block)
	ExpectShouldBe(sizeof(uint64_t) * (maxAllocations), allocator.Allocated)

	return true;
}

char LinearAllocatorTest::MultiAllocationAllSpaceThenFree()
{
	constexpr uint64_t maxAllocations = 1024;
	Owl::LinearAllocator allocator{sizeof(uint64_t) * maxAllocations, nullptr};

	for (uint64_t i = 0; i < maxAllocations; ++i)
	{
		void* block = allocator.Allocate(sizeof(uint64_t));

		ExpectShouldNotBe(nullptr, block)
		ExpectShouldBe(sizeof(uint64_t) * (i + 1), allocator.Allocated)
	}

	allocator.FreeAll();
	ExpectShouldBe(0, allocator.Allocated)

	return true;
}
