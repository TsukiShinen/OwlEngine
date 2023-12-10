#include "TestManager.h"
#include "Memory/LinearAllocatorTest.h"
#include "Owl/Debug/Log.h"

int main()
{
	Owl::Log::Initialize();
	auto testManager = TestManager();

	LinearAllocatorTest::Tests(testManager);

	testManager.RunTests();

	return 0;
}
