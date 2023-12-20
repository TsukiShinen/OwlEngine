#pragma once
#include "VulkanContext.h"

namespace Owl
{
	class VulkanBuffer
	{
	public:
		VulkanBuffer(VulkanContext* pContext, uint64_t pSize, VkFlags pUsage, VkFlags pMemoryPropertyFlags);
		~VulkanBuffer();

		bool Resize(uint64_t pNewSize, VkQueue pQueue, VkCommandPool pPool);

		void Bind(uint64_t pOffset) const;

		void* LockMemory(uint64_t pOffset, uint64_t pSize, VkFlags pFlags) const;
		void UnlockMemory() const;

		void LoadData(uint64_t pOffset, uint64_t pSize, VkFlags pFlags, const void* pData) const;

		static void CopyTo(VulkanContext* pContext, VkCommandPool pPool, VkFence pFence, VkQueue pQueue, VkBuffer pSource, uint64_t pSourceOffset, VkBuffer pDest,
				uint64_t pDestOffset, uint64_t pSize);

	private:
		VulkanContext* m_Context;

		uint64_t m_TotalSize;
		VkBuffer m_Handle;
		VkFlags m_Usage;
		
		bool m_IsLocked;

		VkDeviceMemory m_Memory;
		int m_MemoryIndex;
		uint32_t m_MemoryPropertyFlags;
	};
	
}
