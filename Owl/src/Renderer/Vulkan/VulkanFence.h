#pragma once

#include "VulkanContext.h"

namespace Owl
{
	class VulkanFence
	{
	public:
		VulkanFence(VulkanContext* pContext, bool pCreateSignaled);
		~VulkanFence();
		
		void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, MemoryTagRenderer); }
		void operator delete(void* pBlock) { OWL_FREE(pBlock, sizeof(pBlock), MemoryTagRenderer); }

		bool Wait(uint64_t pTimoutNanoSecond);
		void Reset();

		VkFence& GetHandle() { return m_Handle; }

	private:
		VulkanContext* m_Context;
		VkFence m_Handle;

		bool m_IsSignaled;
	};
	
}
