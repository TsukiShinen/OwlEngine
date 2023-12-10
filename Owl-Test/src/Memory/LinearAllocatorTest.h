#pragma once
#include "../TestManager.h"

class LinearAllocatorTest
{
public:
	static void Tests(TestManager& pTestManager);

private:
	static char SingleAllocationAllSpace();
	static char MultiAllocationAllSpace();
	static char MultiAllocationOverAllocate();
	static char MultiAllocationAllSpaceThenFree();
};
