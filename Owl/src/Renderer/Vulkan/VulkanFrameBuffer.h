﻿#pragma once

#include "VulkanContext.h"
#include "VulkanRenderPass.h"

namespace Owl
{
	class VulkanFrameBuffer
	{
	public:
		VulkanFrameBuffer(VulkanContext* pContext, VulkanRenderPass* pRenderPass, uint32_t pWidth, uint32_t pHeight,
		                  const std::vector<VkImageView>& pAttachments);
		~VulkanFrameBuffer();

		void* operator new(const size_t pSize) { return OWL_ALLOCATE(pSize, Owl::MemoryTagRenderer); }

		void operator delete(void* pBlock, const size_t pSize)
		{
			return OWL_FREE(pBlock, pSize, Owl::MemoryTagRenderer);
		}

		VkFramebuffer& GetHandle() { return m_Handle; }

	private:
		VulkanContext* m_Context;

		VkFramebuffer m_Handle;
		std::vector<VkImageView> m_Attachments;
		VulkanRenderPass* m_RenderPass;
	};
}
