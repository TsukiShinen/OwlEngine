#pragma once

#define ByPass 2
#include <vector>

typedef char (*Test)();

class TestManager
{
public:
	void RegisterTest(Test pTest, const char* pDescription);
	void RunTests();

private:
	struct TestEntry
	{
		Test Function;
		const char* Description;
	};
	
	std::vector<TestEntry> m_Tests;
};
