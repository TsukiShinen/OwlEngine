#include "TestManager.h"
#include "Owl/Debug/Log.h"

int main()
{
	uint64_t logSize;
	Owl::Log::Initialize(&logSize, nullptr);
	auto testManager = TestManager();


	testManager.RunTests();

	return 0;
}
