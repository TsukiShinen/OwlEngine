#pragma once
#include "VulkanContext.h"

namespace Owl
{
	class VulkanImage
	{
	public:
		VulkanImage(VulkanContext* pContext, uint32_t pWidth, uint32_t pHeight, VkFormat pFormat, VkImageTiling pTiling,
		            VkImageUsageFlags pUsage, VkMemoryPropertyFlags pMemoryFlags, bool pCreateView,
		            VkImageAspectFlags pAspectFlags);
		~VulkanImage();
		
		void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, Owl::MemoryTagRenderer); }
		void operator delete(void* pBlock, const size_t pSize) { return OWL_FREE(pBlock, pSize, Owl::MemoryTagRenderer); }

	private:
		void CreateView(VkFormat pFormat, VkImageAspectFlags pAspectFlags);

		VulkanContext* m_Context;

		VkImage m_Handle;
		VkDeviceMemory m_Memory;
		VkImageView m_View;
		uint32_t m_Width;
		uint32_t m_Height;
		friend class VulkanContext;
	};
}
