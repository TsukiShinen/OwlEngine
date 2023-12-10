#include "VulkanFrameBuffer.h"

#include <stdexcept>

#include "VulkanDevice.h"

namespace Owl
{
	VulkanFrameBuffer::VulkanFrameBuffer(VulkanContext* pContext, VulkanRenderPass* pRenderPass, const uint32_t pWidth,
	                                     const uint32_t pHeight, std::vector<VkImageView>& pAttachments)
		: m_Context(pContext), m_Attachments(pAttachments), m_RenderPass(pRenderPass)
	{
		VkFramebufferCreateInfo framebufferCreateInfo = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
		framebufferCreateInfo.renderPass = m_RenderPass->GetHandle();
		framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(m_Attachments.size());
		framebufferCreateInfo.pAttachments = m_Attachments.data();
		framebufferCreateInfo.width = pWidth;
		framebufferCreateInfo.height = pHeight;
		framebufferCreateInfo.layers = 1;

		if (vkCreateFramebuffer(m_Context->Device->GetLogicalDevice(), &framebufferCreateInfo, m_Context->Allocator,
		                        &m_Handle) != VK_SUCCESS)
			throw std::runtime_error("[VulkanFrameBuffer] Failed to create framebuffer!");
	};

	VulkanFrameBuffer::~VulkanFrameBuffer()
	{
		vkDestroyFramebuffer(m_Context->Device->GetLogicalDevice(), m_Handle, m_Context->Allocator);
	}
}
