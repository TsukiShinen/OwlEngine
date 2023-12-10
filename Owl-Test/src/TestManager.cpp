#include "TestManager.h"

#include <ctime>

#include "Owl/Debug/Log.h"

void TestManager::RegisterTest(const Test pTest, const char* pDescription)
{
	m_Tests.push_back({pTest, pDescription});
}

void TestManager::RunTests()
{
	uint32_t passed = 0;
	uint32_t failed = 0;
	uint32_t skipped = 0;

	auto count = static_cast<uint32_t>(m_Tests.size());

	for (uint32_t i = 0; i < count; ++i)
	{
		if (const char result = m_Tests[i].Function(); result == true)
			++passed;
		else if (result == ByPass)
		{
			OWL_WARN("[TestManager] [SKIPPED]: {0}", m_Tests[i].Description);
			++skipped;
		}
		else
		{
			OWL_ERROR("[TestManager] [FAILED]: {0}", m_Tests[i].Description);
			++failed;
		}
		OWL_INFO("[TestManager] Executed {0} of {1} (skipped {2})", i + 1, count, skipped); 
	}

	OWL_INFO("[TestManager] Result: {0} passed, {1} failed, {2} skipped", passed, failed, skipped);
}
