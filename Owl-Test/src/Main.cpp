#include "TestManager.h"
#include "Memory/LinearAllocatorTest.h"
#include "Owl/Debug/Log.h"
#include "Owl/Memory/Memory.h"

int main()
{
	uint64_t logSize;
	Owl::Log::Initialize(&logSize, nullptr);
	Owl::Log::Initialize(&logSize, OWL_ALLOCATE(logSize, Owl::MemoryTagApplication));
	auto testManager = TestManager();

	LinearAllocatorTest::Tests(testManager);

	testManager.RunTests();

	return 0;
}
