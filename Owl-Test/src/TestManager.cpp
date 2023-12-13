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
			OWL_WARN("[TestManager] [SKIPPED]: %s", m_Tests[i].Description);
			++skipped;
		}
		else
		{
			OWL_ERROR("[TestManager] [FAILED]: %s", m_Tests[i].Description);
			++failed;
		}
		OWL_INFO("[TestManager] Executed %d of %d (skipped %d)", i + 1, count, skipped); 
	}

	OWL_INFO("[TestManager] Result: %d passed, %d failed, %d skipped", passed, failed, skipped);
}
