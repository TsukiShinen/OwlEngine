#include "VulkanCommandBuffer.h"

#include <stdexcept>

#include "VulkanDevice.h"
#include "Owl/Debug/Instrumentor.h"

namespace Owl
{
	VulkanCommandBuffer::VulkanCommandBuffer(VulkanContext* pVulkanContext, VkCommandPool& pPool, bool pIsPrimary)
		: m_Context(pVulkanContext), m_Pool(pPool)
	{
		OWL_PROFILE_FUNCTION();
		VkCommandBufferAllocateInfo allocateInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
		allocateInfo.commandPool = m_Pool;
		allocateInfo.level = pIsPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
		allocateInfo.commandBufferCount = 1;
		allocateInfo.pNext = nullptr;

		if (vkAllocateCommandBuffers(m_Context->Device->GetLogicalDevice(), &allocateInfo, &m_Handle) != VK_SUCCESS)
			throw std::runtime_error("[VulkanCommandBuffer] Failed to allocate CommandBuffer!");
		m_State = CommandBufferStateReady;
	}

	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
		OWL_PROFILE_FUNCTION();
		vkFreeCommandBuffers(m_Context->Device->GetLogicalDevice(), m_Pool, 1, &m_Handle);
	}

	void VulkanCommandBuffer::Begin(const bool pIsRenderPassContinue, const bool pIsSimultaneousUse, const bool pIsSingUse)
	{
		OWL_PROFILE_FUNCTION();
		VkCommandBufferBeginInfo beginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
		beginInfo.flags = 0;
		if (pIsSingUse) {
			beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		}
		if (pIsRenderPassContinue) {
			beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		}
		if (pIsSimultaneousUse) {
			beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		}

		if (vkBeginCommandBuffer(m_Handle, &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("[VulkanCommandBuffer] Failed to Begin CommandBuffer!");
		m_State = CommandBufferStateRecording;
	}

	void VulkanCommandBuffer::End()
	{
		OWL_PROFILE_FUNCTION();
		if (vkEndCommandBuffer(m_Handle) != VK_SUCCESS)
			throw std::runtime_error("[VulkanCommandBuffer] Failed to End CommandBuffer!");
		m_State = CommandBufferStateRecordingEnded;
	}

	void VulkanCommandBuffer::Submitted()
	{
		OWL_PROFILE_FUNCTION();
		m_State = CommandBufferStateSubmitted;
	}

	void VulkanCommandBuffer::Reset()
	{
		OWL_PROFILE_FUNCTION();
		m_State = CommandBufferStateReady;
	}

	VulkanCommandBuffer* VulkanCommandBuffer::CreateAndBeginSingleTimeUse(VulkanContext* pVulkanContext,
		VkCommandPool& pPool)
	{
		OWL_PROFILE_FUNCTION();
		const auto commandBuffer = new VulkanCommandBuffer(pVulkanContext, pPool, true);
		commandBuffer->Begin(false, false, true);
		return commandBuffer;
	}

	void VulkanCommandBuffer::DeleteAndEndSingleTimeUse(VulkanContext* pVulkanContext, VkCommandPool& pPool,
		VulkanCommandBuffer* pCommandBuffer, VkQueue pQueue)
	{
		OWL_PROFILE_FUNCTION();
		pCommandBuffer->End();

		VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &pCommandBuffer->m_Handle;
		if (vkQueueSubmit(pQueue, 1, &submitInfo, nullptr) != VK_SUCCESS)
			throw std::runtime_error("[VulkanCommandBuffer] Failed to submit queue!");

		if (vkQueueWaitIdle(pQueue) != VK_SUCCESS)
			throw std::runtime_error("[VulkanCommandBuffer] Failed to wait for queue!");

		delete pCommandBuffer;
	}
}
