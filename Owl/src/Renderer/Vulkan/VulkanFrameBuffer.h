#pragma once

#include "VulkanContext.h"
#include "VulkanRenderPass.h"

namespace Owl
{
	class VulkanFrameBuffer
	{
	public:
		VulkanFrameBuffer(VulkanContext* pContext, VulkanRenderPass* pRenderPass, uint32_t pWidth, uint32_t pHeight, std::vector<VkImageView>& pAttachments);
		~VulkanFrameBuffer();
		
		void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, MemoryTagRenderer); }
		void operator delete(void* pBlock) { OWL_FREE(pBlock, sizeof(pBlock), MemoryTagRenderer); }

	private:
		VulkanContext* m_Context;
		
		VkFramebuffer m_Handle;
		std::vector<VkImageView>& m_Attachments;
		VulkanRenderPass* m_RenderPass;
	};
	
}
