#include "VulkanRenderPass.h"

#include <stdexcept>

#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "Owl/Debug/Log.h"
#include "Owl/Math/Vector4.h"

namespace Owl
{
	VulkanRenderPass::VulkanRenderPass(VulkanContext* pContext, const Vector4 pRect)
		: m_Context(pContext)
	{
		// Main subpass
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

		// Attachments TODO: make this configurable.
		uint32_t attachmentDescriptionCount = 2;
		std::vector<VkAttachmentDescription> attachmentDescriptions(attachmentDescriptionCount);

		// Color attachment
		VkAttachmentDescription colorAttachment;
		colorAttachment.format = m_Context->Swapchain->GetImageFormat().format; // TODO: configurable
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		// Do not expect any particular layout before render pass starts.
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Transitioned to after the render pass
		colorAttachment.flags = 0;

		attachmentDescriptions[0] = colorAttachment;

		VkAttachmentReference colorAttachmentReference;
		colorAttachmentReference.attachment = 0; // Attachment description array index
		colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentReference;

		// Depth attachment, if there is one
		VkAttachmentDescription depthAttachment = {};
		depthAttachment.format = m_Context->Device->GetDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		attachmentDescriptions[1] = depthAttachment;

		// Depth attachment reference
		VkAttachmentReference depth_attachment_reference;
		depth_attachment_reference.attachment = 1;
		depth_attachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		// TODO: other attachment types (input, resolve, preserve)

		// Depth stencil data.
		subpass.pDepthStencilAttachment = &depth_attachment_reference;

		// Input from a shader
		subpass.inputAttachmentCount = 0;
		subpass.pInputAttachments = nullptr;

		// Attachments used for multisampling colour attachments
		subpass.pResolveAttachments = nullptr;

		// Attachments not used in this subpass, but must be preserved for the next.
		subpass.preserveAttachmentCount = 0;
		subpass.pPreserveAttachments = nullptr;

		// Render pass dependencies. TODO: make this configurable.
		VkSubpassDependency dependency;
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.dependencyFlags = 0;

		// Render pass create.
		VkRenderPassCreateInfo renderPassCreateInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
		renderPassCreateInfo.attachmentCount = attachmentDescriptionCount;
		renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpass;
		renderPassCreateInfo.dependencyCount = 1;
		renderPassCreateInfo.pDependencies = &dependency;
		renderPassCreateInfo.pNext = nullptr;
		renderPassCreateInfo.flags = 0;

		if (vkCreateRenderPass(m_Context->Device->GetLogicalDevice(), &renderPassCreateInfo, m_Context->Allocator,
		                       &m_Handle) != VK_SUCCESS)
			throw std::runtime_error("Failed to create Vulkan render pass!");

		m_Info = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
		m_Info.renderPass = m_Handle;
		m_Info.renderArea.offset.x = pRect.x;
		m_Info.renderArea.offset.y = pRect.y;
		m_Info.renderArea.extent.width = pRect.z;
		m_Info.renderArea.extent.height = pRect.w;

		SetClearColor({0.2f, 0.2f, 0.2f, 1.f});
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		if (m_Handle)
			vkDestroyRenderPass(m_Context->Device->GetLogicalDevice(), m_Handle, m_Context->Allocator);
	}

	void VulkanRenderPass::SetClearColor(const Vector4 pColor, const float pDepth, const uint32_t pStencil)
	{
		m_ClearValues[0].color.float32[0] = pColor.r;
		m_ClearValues[0].color.float32[1] = pColor.g;
		m_ClearValues[0].color.float32[2] = pColor.b;
		m_ClearValues[0].color.float32[3] = pColor.a;
		m_ClearValues[1].depthStencil.depth = pDepth;
		m_ClearValues[1].depthStencil.stencil = pStencil;

		m_Info.clearValueCount = 2;
		m_Info.pClearValues = m_ClearValues;
	}

	void VulkanRenderPass::Begin(VulkanCommandBuffer* pCommandBuffer, const VkFramebuffer pFramebuffer)
	{
		m_Info.framebuffer = pFramebuffer;
		vkCmdBeginRenderPass(pCommandBuffer->GetHandle(), &m_Info, VK_SUBPASS_CONTENTS_INLINE);
		pCommandBuffer->SetState(CommandBufferStateInRenderPass);
	}

	void VulkanRenderPass::End(VulkanCommandBuffer* pCommandBuffer)
	{
		vkCmdEndRenderPass(pCommandBuffer->GetHandle());
		pCommandBuffer->SetState(CommandBufferStateRecording);
	}
}
