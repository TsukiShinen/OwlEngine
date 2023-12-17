#pragma once

#include "VulkanContext.h"

namespace Owl
{
	class VulkanFence
	{
	public:
		VulkanFence(VulkanContext* pContext, bool pCreateSignaled);
		~VulkanFence();

		void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, Owl::MemoryTagRenderer); }

		void operator delete(void* pBlock, const size_t pSize)
		{
			return OWL_FREE(pBlock, pSize, Owl::MemoryTagRenderer);
		}

		bool Wait(uint64_t pTimoutNanoSecond);
		void Reset();

		VkFence& GetHandle() { return m_Handle; }

	private:
		VulkanContext* m_Context;
		VkFence m_Handle;

		bool m_IsSignaled;
	};
}
